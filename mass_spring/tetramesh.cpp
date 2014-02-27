#include "tetramesh.h"

struct	TetraCube
{
    #include "cube.inl"
};

struct OneTetra
{
    #include "onetetra.inl"
};

struct	TetraCube8
{
    #include "cube8.inl"
};

TetraMesh::TetraMesh(const char *nodefile, const char *elefile, float scale)
{

    //read nodes and elements from files
//    readVertexs(nodefile);
//    std::cout<<"Nodes file:"<<nodefile<<" readed."<<std::endl;
//    const char *node =  nodebuffer;
//    readElements(elefile);
//    std::cout<<"Elements file:"<<elefile<<" readed."<<std::endl;
//    const char *tetra = elebuffer;

    const char *tetra =TetraCube::getElements();
    const char *node = TetraCube::getNodes();

    nnode=0; int ndims=0; int nattrb=0; int hasbounds=0;
    sscanf(node,"%d %d %d %d",&nnode,&ndims,&nattrb,&hasbounds);    
    sscanf(node,"%d %d %d %d",&nnode,&ndims,&nattrb,&hasbounds);

    node += nextLine(node);
    //read vertexs
    m_verts = new float[3*nnode];
    m_rowSize = std::ceil(std::sqrt(nnode));
    m_frowSize = std::ceil(std::sqrt(3*nnode));
    m_position = (float*) calloc( 4*m_rowSize*m_rowSize + 4,sizeof(float));
    int vindex = 0;
    for(uint v = 0; v < nnode; v++){
        int	index=0;
        float x,y,z;
        sscanf(node,"%d %g %g %g",&index,&x,&y,&z);
        node += nextLine(node);
        m_verts[vindex] = scale*x;
        vindex++;
        m_verts[vindex] = scale*y;
        vindex++;
        m_verts[vindex] = scale*z;
        vindex++;
    }
    std::cout<<nnode<<" vertexs loaded."<<std::endl;
    //read tetrahedral
    if(tetra && tetra[0]){
        ntetra=0; int ncorner=0; int neattrb=0;
        sscanf( tetra, "%d %d %d", &ntetra, &ncorner, &neattrb);
        tetra += nextLine(tetra);        
        m_indexs = new uint[4*ntetra];
        m_rvolum = new float[ntetra];        
        int tindex = 0;
        for(uint t = 0; t < ntetra; t++){
            int	index=0;
            uint ni[4];
            sscanf( tetra, "%d %d %d %d %d", &index, &ni[0], &ni[1], &ni[2], &ni[3]);
            tetra += nextLine(tetra);
            memcpy( &m_indexs[tindex], ni, 4*sizeof(uint));
            tindex += 4;
            uint i0 = 3*ni[0]; uint i1 = 3*ni[1];
            uint i2 = 3*ni[2]; uint i3 = 3*ni[3];
            glm::vec3 x0(m_verts[i0], m_verts[i0+1], m_verts[i0+2]);
            glm::vec3 x1(m_verts[i1], m_verts[i1+1], m_verts[i1+2]);
            glm::vec3 x2(m_verts[i2], m_verts[i2+1], m_verts[i2+2]);
            glm::vec3 x3(m_verts[i3], m_verts[i3+1], m_verts[i3+2]);
            //tetrahedron volume
            m_rvolum[t] = VolumeOf( x0, x1, x2, x3 );
        }
    }
    std::cout<<ntetra<<" tetrahedral loaded."<<std::endl;
    findEdges(); //Find mesh edges
    std::cout<<nedge<<" edges loaded."<<std::endl;
    //locate memory
    m_force = (float*) calloc( m_frowSize*m_frowSize + 3,sizeof(float));
    m_velocity = (float*) calloc( 4*m_rowSize*m_rowSize + 4,sizeof(float));
    m_prevpos = (float*) calloc( 4*m_rowSize*m_rowSize + 4,sizeof(float));
    m_imass = (float*) calloc(m_rowSize*m_rowSize,sizeof(float));
    setVolumeMass(10.0f);
}

TetraMesh::~TetraMesh(){
    delete[]m_verts;
    delete[]m_position;
    delete[]m_prevpos;
    delete[]m_force;
    delete[]m_velocity;
}

void TetraMesh::sort4(uint *a){
    uint tmp;
    if( a[0] > a[1] ){
        tmp = a[0];
        a[0] = a[1];
        a[1] =tmp;
    }
    if( a[2] > a[3] ){
        tmp = a[2];
        a[2] = a[3];
        a[3] = tmp;
    }
    if( a[0] > a[2] ){
        tmp = a[0];
        a[0] = a[2];
        a[2]= tmp;
    }
    if( a[1] > a[3] ){
        tmp = a[1];
        a[1] = a[3];
        a[3] = tmp;
    }
    if(a[1] > a[2] ){
        tmp = a[1];
        a[1] = a[2];
        a[2] = tmp;
    }
}

int TetraMesh::check_index(std::vector<uint>& v, uint index, int i, int f){
    if(v.size()==0)
        return 0;
    else if(i==f){
        if(index < v[i])
            return i;
        else if(index > v[i])
            return i+1;
        return -1;
    }
    else if(i+1 == f){
        if(v[i]>index)
            return i;
        else if(v[f] < index)
            return f+1;
        if( v[i]< index && v[f]>index )
            return i+1;
        return -1;
    }
    else{
        uint m = (i+f)/2;
        if(index < v[m])
            return check_index(v, index, i, m);
        else if(index > v[m])
            return check_index(v, index, m, f);
        return -1;
    }
}

void TetraMesh::add_edges(std::vector<uint>& v, uint *edges, uint count){
    std::vector<uint>::iterator it = v.begin();
    int index;
    int i = 0; int f = v.size()-1;
    while(count){
        if( (index = check_index(v, *edges, i, f  )) > -1 ){
            v.insert(it + index, *edges);
            i = index; f++;
            nedge++;
            it = v.begin();
        }
        count--;
        edges++;
    }
}

void TetraMesh::findEdges(){
    nedge = 0;
    std::vector<uint> ematrix[nnode - 1];
    uint i,j;
    for(i = 0; i < (nnode - 1); i++ )
        ematrix[i] = std::vector<uint>();

    uint nindexs[4];
    uint tindex = 0;
    uint* e;
    for(i = 0; i < ntetra; i++){
        memcpy( nindexs, &m_indexs[tindex], 4*sizeof(uint));
        tindex+=4;
        sort4(nindexs);
        e = &nindexs[1];
        add_edges(ematrix[nindexs[0]], e, 3);
        e = &nindexs[2];
        add_edges(ematrix[nindexs[1]], e, 2);
        e = &nindexs[3];
        add_edges(ematrix[nindexs[2]], e);
    }

    m_edges = new uint[2*nedge];
    uint eindex = 0;
    for(i = 0; i < (nnode - 1); i++ ){
        for(j = 0; j < ematrix[i].size(); j++){
            m_edges[eindex++] = i;
            m_edges[eindex++] = ematrix[i][j];
        }
    }
    //        ////debug///////////////////////////////////////////////////////
    //        std::cout<<"Edges "<< nedge << ": ";
    //        for(uint x = 0; x < 2*nedge; x++){
    //            std::cout<< m_edges[x++] <<"-";
    //            std::cout<< m_edges[x]<<", ";
    //        }
    //        std::cout<<std::endl;
    //        ////////////////////////////////////////////////////////////////
}

float TetraMesh::getTotalMass(){
    float mass = 0.0f;
    for(uint i=0; i < nnode; i++){
        mass += (m_imass[i] > 0)? 1/m_imass[i] : 0;
    }
    return mass;
}

void TetraMesh::setTotalMass(float mass)
{
    const float	tm=getTotalMass();
    const float	itm=1/tm;
    for(uint i=0;i<nnode;i++)
    {
        m_imass[i]/=itm*mass;
    }
}

void TetraMesh::setVolumeMass(float mass)
{
    std::vector<float> ranks(nnode,0.0f);
    uint i;

    for(i=0;i<ntetra;i++)
    {
        uint t = 3*i;
        for(uint j=0; j<4; j++)
        {
            m_imass[m_indexs[t + j]] += std::fabs(m_rvolum[i]);
            ranks[m_indexs[t + j]] += 1;
        }
    }
    for( i=0;i<nnode;i++)
    {
        if(m_imass[i] > 0)
        {
            m_imass[i] = (ranks[i]/m_imass[i]);
        }
    }
    setTotalMass(mass);
}

int TetraMesh::nextLine(const char* buffer)
{
    int numBytesRead=0;

    while (*buffer != '\n')
    {
        buffer++;
        numBytesRead++;
    }

    if (buffer[0]==0x0a)
    {
        buffer++;
        numBytesRead++;
    }
    return numBytesRead;
}

void TetraMesh::rotate(float angle, glm::vec3 axis){
    m_transform = glm::rotate(m_transform, angle, axis);
    m_invtransform = glm::inverse(m_transform);
}

void TetraMesh::translate(glm::vec3 translation){
    m_transform = glm::translate(m_transform,translation);
    m_invtransform = glm::inverse(m_transform);
}

void TetraMesh::readElements(const char* elefile)
    {
        FILE *pFile = fopen ( elefile , "r" );
        if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
        // obtain file size:
        fseek (pFile , 0 , SEEK_END);
        ulong lSize = ftell (pFile);
        rewind(pFile);
        // allocate memory to contain the whole file:
        elebuffer = (char*) malloc (sizeof(char)*lSize);
        if (elebuffer == NULL) {fputs ("Memory error",stderr); exit (2);}
        // copy the file into the buffer:
        size_t result = fread (elebuffer,1,lSize,pFile);
        if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
    }

void TetraMesh::readVertexs(const char* nodefile){
    FILE *pFile = fopen ( nodefile , "r" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    ulong lSize = ftell (pFile);
    rewind(pFile);
    // allocate memory to contain the whole file:
    nodebuffer = (char*) malloc (sizeof(char)*lSize);
    if (nodebuffer == NULL) {fputs ("Memory error",stderr); exit (2);}
    // copy the file into the buffer:
    size_t result = fread (nodebuffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
}
