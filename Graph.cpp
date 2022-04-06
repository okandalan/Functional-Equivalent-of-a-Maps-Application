#include "Graph.h"
#include "GraphExceptions.h"

#include <iostream>
#include <iomanip>
#include <queue>
#include <fstream>
#include <sstream>
#include <cstdlib>

// Literally do nothing here
// default constructors of the std::vector is enough
Graph::Graph()
{}

Graph::Graph(const std::string& filePath)
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    // Tokens
    std::string tokens[3];

    std::ifstream mapFile(filePath.c_str());
    // Read line by line
    std::string line;
    while (std::getline(mapFile, line))
    {
        // Empty Line Skip
        if(line.empty()) continue;
        // Comment Skip
        if(line[0] == '#') continue;

        // Tokenize the line
        int i = 0;
        std::istringstream stream(line);
        while(stream >> tokens[i]) i++;

        // Single token (Meaning it is a vertex)
        if(i == 1)
        {
            InsertVertex(tokens[0]);
        }
        // Exactly three tokens (Meaning it is an edge)
        else if(i == 3)
        {
            int weight = std::atoi(tokens[0].c_str());
            if(!ConnectVertices(tokens[1], tokens[2], weight))
            {
                std::cerr << "Duplicate edge on "
                          << tokens[0] << "-"
                          << tokens[1] << std::endl;
            }
        }
        else std::cerr << "Token Size Mismatch" << std::endl;
    }
}

void Graph::InsertVertex(const std::string& vertexName)
{
    // TODO
    int size = vertexList.size();
    for (int i = 0; i < size; i++) {
        if (vertexList[i].name == vertexName) {
            throw DuplicateVertexNameException();
        }
    }
    GraphVertex new_vertex;
    new_vertex.name = vertexName;
    new_vertex.edgeCount = 0;
    vertexList.push_back(new_vertex);
}
bool Graph::ConnectVertices(const std::string& fromVertexName,
                            const std::string& toVertexName,
                            int weight)
{
    int index1, index2;
    index1 = index2 = -1;
    int vertex_list_size = vertexList.size();
    for (int i = 0; i < vertex_list_size; i++) {
        if (index1 != -1 && index2 != -1) {
            break;
        }      

        if (vertexList[i].name == fromVertexName) {
            index1 = i;
        }

        if (vertexList[i].name == toVertexName) {
            index2 = i;
        }
    }

    if (index1 == -1 || index2 == -1) {
        throw VertexNotFoundException();
    }

    if (index1 == index2)
        return false;

    int edge_count1 = vertexList[index1].edgeCount;
    for (int i = 0; i < edge_count1; i++) {
        int egde_index = vertexList[index1].edgeIds[i];
        if (edgeList[egde_index].vertexId0 == index2 || edgeList[egde_index].vertexId1 == index2) {
            return false;
        }
    }


    if (vertexList[index1].edgeCount == MAX_EDGE_PER_VERTEX || vertexList[index2].edgeCount == MAX_EDGE_PER_VERTEX) {
        throw TooManyEdgeOnVertexExecption();
    }

    GraphEdge new_edge;
    new_edge.weight = weight;
    new_edge.masked = false;
    new_edge.vertexId0 = index1;
    new_edge.vertexId1 = index2;

    int edge_list_size = edgeList.size();
    int edge_count2 = vertexList[index2].edgeCount;


    vertexList[index1].edgeIds[edge_count1] = edge_list_size;
    vertexList[index2].edgeIds[edge_count2] = edge_list_size;


    edgeList.push_back(new_edge);
    vertexList[index1].edgeCount++;
    vertexList[index2].edgeCount++;



    return true;
}



bool Graph::ShortestPath(std::vector<int>& orderedVertexIdList,
                         const std::string& from,
                         const std::string& to) const
{
    orderedVertexIdList.clear();
    int from_index, to_index;
    from_index = to_index = -1;
    int vertex_list_size = vertexList.size();
    for (int i = 0; i < vertex_list_size; i++) {
        if (vertexList[i].name == from) {
            from_index = i;
        }
        if (vertexList[i].name == to) {
            to_index = i;
        }        
    }
    
    if (from_index == -1 || to_index == -1) {
        throw VertexNotFoundException();
    }
    
    if (from_index == to_index) {
        orderedVertexIdList.push_back(from_index);
        return true;
    }

    std::priority_queue<DistanceVertexIdPair> pq;
    
    int * distance;
    int * prev;
    distance = new int[vertex_list_size];
    prev = new int[vertex_list_size];
    for (int i = 0; i < vertex_list_size; i++) {
        distance[i] = LARGE_NUMBER;
        prev[i] = -1; 
    }
    
    distance[from_index] = 0;

    for (int i = 0; i < vertex_list_size; i++) {
        pq.push(DistanceVertexIdPair(i, distance[i]));
    }

    while (!pq.empty()) {
        DistanceVertexIdPair v = pq.top();
        pq.pop();
        if (v.currentWeight == LARGE_NUMBER) {
            break;
        }
        for (int i = 0; i < vertexList[v.vId].edgeCount; i++) {
            GraphEdge e = edgeList[vertexList[v.vId].edgeIds[i]];
            if (e.masked) {

                continue;
            }
            int neighbor_index;
            neighbor_index = e.vertexId0 != v.vId ? e.vertexId0 : e.vertexId1;
            
            int new_distance = distance[v.vId] + e.weight;
            if (new_distance < distance[neighbor_index]) {
                distance[neighbor_index] = new_distance;
                prev[neighbor_index] = v.vId;
                DistanceVertexIdPair decreased_vertex(neighbor_index, new_distance);
                pq.push(decreased_vertex);
            }
        }
    }

    if (distance[to_index] == LARGE_NUMBER) {
        delete [] distance;
        delete [] prev;
        return false;
    }

    delete [] distance;
    
    std::vector<int> reversed_path;
    reversed_path.push_back(to_index);
    int prev_vertex = to_index;
    while (true) {

        prev_vertex = prev[prev_vertex];
        reversed_path.push_back(prev_vertex);
        if (prev_vertex == from_index) {
            break;
        }
    }
    
    int path_size = reversed_path.size();
    for (int i = path_size - 1; i >= 0; i--) {
        orderedVertexIdList.push_back(reversed_path[i]);
    }

    delete [] prev;
    reversed_path.clear();


    return true;

}

int Graph::MultipleShortPaths(std::vector<std::vector<int> >& orderedVertexIdList,
                              const std::string& from,
                              const std::string& to,
                              int numberOfShortestPaths)
{
    UnMaskAllEdges();
    std::vector<int> LastPath;
    for (int i = 0; i < numberOfShortestPaths; i++) {
        if (!ShortestPath(LastPath, from, to)) {
            break;
        }

        if (from == to) {
            break;
        }

        orderedVertexIdList.push_back(LastPath);
        int path_size = LastPath.size();
        int highest_id = -1;
        int highest_weight = -1;
        for (int i = 0; i < path_size - 1; i++) {
            int v1_id = LastPath[i];
            int v2_id = LastPath[i+1];

            GraphVertex v1 = vertexList[v1_id];
            GraphEdge edge;
            for (int j = 0; j < v1.edgeCount; j++) {
                edge = edgeList[v1.edgeIds[j]];
                if (edge.vertexId0 == v2_id || edge.vertexId1 == v2_id) {
                    if (edge.weight > highest_weight) {
                        highest_weight = edge.weight;
                        highest_id = v1.edgeIds[j];
                    }
                    break;
                }
            }
        }
        edgeList[highest_id].masked = true;
    }

    UnMaskAllEdges();
    return orderedVertexIdList.size();
}

void Graph::MaskEdges(const std::vector<StringPair>& vertexNames)
{
    int index1, index2;
    index1 = index2 = -1;

    int vertex_list_size = vertexList.size();
    int vertex_pairs_size = vertexNames.size();
    for (int j = 0; j < vertex_pairs_size; j++) {
        index1 = index2 = -1;
        for (int i = 0; i < vertex_list_size; i++) {
            if (index1 != -1 && index2 != -1) {
                break;
            }

            if (vertexList[i].name == vertexNames[j].s0) {
                index1 = i;
            }
            
            if (vertexList[i].name == vertexNames[j].s1) {
                index2 = i;
            }
        }

        if (index1 == -1 || index2 == -1) {
            throw VertexNotFoundException();
        } 


        int edge_count1 = vertexList[index1].edgeCount;
        for (int k = 0; k < edge_count1; k++) {
            int egde_index = vertexList[index1].edgeIds[k];
            if (edgeList[egde_index].vertexId0 == index2 || edgeList[egde_index].vertexId1 == index2) {
                edgeList[egde_index].masked = true;
                break;
            }
        }        
    }
   
}

void Graph::UnMaskEdges(const std::vector<StringPair>& vertexNames)
{
    int index1, index2;
    

    int vertex_list_size = vertexList.size();
    int vertex_pairs_size = vertexNames.size();
    for (int j = 0; j < vertex_pairs_size; j++) {
        index1 = index2 = -1;
        for (int i = 0; i < vertex_list_size; i++) {
            if (index1 != -1 && index2 != -1) {
                break;
            }

            if (vertexList[i].name == vertexNames[j].s0) {
                index1 = i;
            }
            
            if (vertexList[i].name == vertexNames[j].s1) {
                index2 = i;
            }
        }

        if (index1 == -1 || index2 == -1) {
            throw VertexNotFoundException();
        }
        
        if (index1 == index2)
            continue;

        int edge_count1 = vertexList[index1].edgeCount;
        for (int k = 0; k < edge_count1; k++) {
            int egde_index = vertexList[index1].edgeIds[k];
            if (edgeList[egde_index].vertexId0 == index2 || edgeList[egde_index].vertexId1 == index2) {
                edgeList[egde_index].masked = false;
                break;
            }
        } 
    }
}

void Graph::UnMaskAllEdges()
{
    int size = edgeList.size();
    for (int i = 0; i < size; i++) {
        edgeList[i].masked = false;
    }
}

void Graph::MaskVertexEdges(const std::string& name)
{
    int vertex_list_size = vertexList.size();
    int index = -1;
    for (int i = 0; i < vertex_list_size; i++) {
        if (vertexList[i].name == name) {
            index = i;
        }
    }

    if (index == -1) {
        throw VertexNotFoundException();
    }

    int edge_count = vertexList[index].edgeCount;
    for (int i = 0; i < edge_count; i++) {
        int egde_index = vertexList[index].edgeIds[i];
        edgeList[egde_index].masked = true;
    }
}

void Graph::UnMaskVertexEdges(const std::string& name)
{
    int vertex_list_size = vertexList.size();
    int index = -1;
    for (int i = 0; i < vertex_list_size; i++) {
        if (vertexList[i].name == name) {
            index = i;
        }
    }

    if (index == -1) {
        throw VertexNotFoundException();
    }

    int edge_count = vertexList[index].edgeCount;
    for (int i = 0; i < edge_count; i++) {
        int egde_index = vertexList[index].edgeIds[i];
        edgeList[egde_index].masked = false;
    }
}

void Graph::ModifyEdge(const std::string& vName0,
                       const std::string& vName1,
                       float newWeight)
{
    int index0, index1;
    index0 = index1 = -1;
    int vertex_list_size = vertexList.size();
    
    for (int i = 0; i < vertex_list_size; i++) {
        if (index0 != -1 && index1 != -1) {
            break;
        }      

        if (vertexList[i].name == vName0) {
            index0 = i;
        }

        if (vertexList[i].name == vName1) {
            index1 = i;
        }
    }

    if (index0 == -1 || index1 == -1) {
        throw VertexNotFoundException();

    }

    int edge_count0 = vertexList[index0].edgeCount;
    for (int k = 0; k < edge_count0; k++) {
        int egde_index = vertexList[index0].edgeIds[k];
        if (edgeList[egde_index].vertexId0 == index1 || edgeList[egde_index].vertexId1 == index1) {
            edgeList[egde_index].weight = newWeight;
            break;
        }
    }
}

void Graph::ModifyEdge(int vId0, int vId1,
                       float newWeight)
{
    int vertex_list_size = vertexList.size();
    if (vId0 < 0 || vId1 < 0 || vId0 >= vertex_list_size || vId1 >= vertex_list_size) {
        return;
    }

    int edge_count0 = vertexList[vId0].edgeCount;
    for (int k = 0; k < edge_count0; k++) {
        int egde_index = vertexList[vId0].edgeIds[k];
        if (edgeList[egde_index].vertexId0 == vId1 || edgeList[egde_index].vertexId1 == vId1) {
            edgeList[egde_index].weight = newWeight;
            break;
        }
    }    
}

void Graph::PrintAll() const
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    for(size_t i = 0; i < vertexList.size(); i++)
    {
        const GraphVertex& v = vertexList[i];
        std::cout << v.name << "\n";
        for(int j = 0; j < v.edgeCount; j++)
        {
            int edgeId = v.edgeIds[j];
            const GraphEdge& edge = edgeList[edgeId];
            // Skip printing this edge if it is masked
            if(edge.masked)
                continue;

            // List the all vertex names and weight
            std::cout << "-" << std::setfill('-')
                             << std::setw(2) << edge.weight
                             << "-> ";
            int neigVertexId = (static_cast<int>(i) == edge.vertexId0)
                                 ? edge.vertexId1
                                 : edge.vertexId0;
            std::cout << vertexList[neigVertexId].name << "\n";
        }
    }
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

void Graph::PrintPath(const std::vector<int>& orderedVertexIdList,
                      bool sameLine) const
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this file !       //
    // ============================= //
    for(size_t i = 0; i < orderedVertexIdList.size(); i++)
    {
        int vertexId = orderedVertexIdList[i];
        if(vertexId >= static_cast<int>(vertexList.size()))
            throw VertexNotFoundException();

        const GraphVertex& vertex = vertexList[vertexId];
        std::cout << vertex.name;
        if(!sameLine) std::cout << "\n";
        // Only find and print the weight if next is available
        if(i == orderedVertexIdList.size() - 1) break;
        int nextVertexId = orderedVertexIdList[i + 1];
        if(nextVertexId >= static_cast<int>(vertexList.size()))
            throw VertexNotFoundException();

        // Find the edge between these two vertices
        int edgeId = INVALID_INDEX;
        if(vertexId     < static_cast<int>(vertexList.size()) &&
           nextVertexId < static_cast<int>(vertexList.size()))
        {
            // Check all of the edges of vertex
            // and try to find
            const GraphVertex& fromVert = vertexList[vertexId];
            for(int i = 0; i < fromVert.edgeCount; i++)
            {
                int eId = fromVert.edgeIds[i];
                // Since the graph is not directional
                // check the both ends
                if((edgeList[eId].vertexId0 == vertexId &&
                    edgeList[eId].vertexId1 == nextVertexId)
                ||
                   (edgeList[eId].vertexId0 == nextVertexId &&
                    edgeList[eId].vertexId1 == vertexId))
                {
                    edgeId = eId;
                    break;
                }
            }
        }
        if(edgeId != INVALID_INDEX)
        {
            const GraphEdge& edge = edgeList[edgeId];
            std::cout << "-" << std::setfill('-')
                      << std::setw(2)
                      << edge.weight << "->";
        }
        else
        {
            std::cout << "-##-> ";
        }
    }
    // Print endline on the last vertex if same line is set
    if(sameLine) std::cout << "\n";
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

int Graph::TotalVertexCount() const
{
    return vertexList.size();
}

int Graph::TotalEdgeCount() const
{
    return edgeList.size();
}

std::string Graph::VertexName(int vertexId) const
{
    if (vertexId < 0 || vertexId >= TotalVertexCount()) {
        return "";
    }

    return vertexList[vertexId].name;
}

int Graph::TotalWeightInBetween(std::vector<int>& orderedVertexIdList)
{
    int total_weight = 0;
    int path_size = orderedVertexIdList.size();
    int vertex_list_size = TotalVertexCount();
    for (int i = 0; i < path_size - 1; i++) {
        int v1_index = orderedVertexIdList[i];
        int v2_index = orderedVertexIdList[i+1];

        if (v1_index >= vertex_list_size || v2_index >= vertex_list_size) {
            VertexNotFoundException();
        }

        GraphVertex v1 = vertexList[v1_index];
        GraphEdge edge;
        for (int j = 0; j < v1.edgeCount; j++) {
            edge = edgeList[v1.edgeIds[j]];
            if (edge.vertexId0 == v2_index || edge.vertexId1 == v2_index) {
                total_weight += edge.weight;
                break;
            }
        }
    }

    if (total_weight == 0) {
        return -1;
    }

    return total_weight;
}