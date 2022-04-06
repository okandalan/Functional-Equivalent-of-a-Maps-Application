#include "METUMaps.h"
#include "GraphExceptions.h"

#include <iostream>

void METUMaps::PrintNotInJourney() const
{
    std::cout << "Device is not in a journey!" << std::endl;
}

void METUMaps::PrintUnableToChangeDestination() const
{
    std::cout << "Cannot change Destination during journey!" << std::endl;
}

void METUMaps::PrintUnableToChangeStartingLoc() const
{
    std::cout << "Cannot change Starting Location during journey!" << std::endl;
}

void METUMaps::PrintAlreadyInJourney() const
{
    std::cout << "Device is already in a journey!" << std::endl;
}

void METUMaps::PrintJourneyIsAlreadFinished() const
{
    std::cout << "Journey is already finished!" << std::endl;
}

void METUMaps::PrintLocationNotFound() const
{
    std::cout << "One (or both) of the locations are not found in the maps!" << std::endl;
}

void METUMaps::PrintJourneyCompleted() const
{
    std::cout << "Journey Completed!" << std::endl;
}

void METUMaps::PrintCachedLocationFound(const std::string& location0,
                                        const std::string& location1) const
{
    std::cout << "Route between \""
              << location0 << "\" and \""
              << location1 << "\" is in cache, using that..."
              << std::endl;
}

void METUMaps::PrintCalculatingRoutes(const std::string& location0,
                                      const std::string& location1) const
{
    std::cout << "Calculating Route(s) between \""
              << location0 << "\" and \""
              << location1 << "\"..."
              << std::endl;
}

std::string METUMaps::GenerateKey(const std::string& location0,
                                  const std::string& location1)
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    return location0 + "/" + location1;
}

METUMaps::METUMaps(int potentialPathCount,
                   const std::string& mapFilePath)
{
    map = Graph(mapFilePath);
    this->potentialPathCount = potentialPathCount;
    cachedPaths = KeyedHashTable(map.TotalVertexCount() * potentialPathCount);
    inJourney = false;
}

void METUMaps::SetDestination(const std::string& name)
{
    if (inJourney) {
        PrintUnableToChangeDestination();
    }

    else {
        destination = name;
    }

}

void METUMaps::SetStartingLocation(const std::string& name)
{
    if (inJourney) {
        PrintUnableToChangeDestination();
    }

    else {
        startingLoc = name;
    }
}

void METUMaps::Caching(const std::vector<std::vector<int> >& PathList) {

    int path_list_size = PathList.size();
    std::vector<int> tmp;
    for (int i = 0; i < path_list_size; i++) {
        int path_size = PathList[i].size();
        for (int j = 0; j < path_size - 1; j++) {
            std::string key = GenerateKey(map.VertexName(PathList[i][j]), destination);
            for (int k = j; k < path_size; k++) {
                tmp.push_back(PathList[i][k]);
            }
            if (cachedPaths.Find(tmp, key)) {
                tmp.clear();
                continue;
            }
            cachedPaths.Insert(key, tmp);
            tmp.clear();
        }
    }

}

void METUMaps::StartJourney()
{
    // TODO
    if (inJourney) {
        PrintAlreadyInJourney();
        return;
    }
    PrintCalculatingRoutes(startingLoc, destination);
    std::vector<std::vector<int> > path_list;
    try
    {
        map.MultipleShortPaths(path_list, startingLoc, destination, potentialPathCount);

    }
    catch(std::runtime_error &e)
    {
        PrintLocationNotFound();
        return;
    }
    
    Caching(path_list);
    currentLoc = startingLoc;
    currentRoute = path_list[0];
    inJourney = true;    
}

void METUMaps::EndJourney()
{
    if (!inJourney) {
        PrintJourneyIsAlreadFinished();
        return;
    }
    inJourney = false;
    cachedPaths.ClearTable();
    startingLoc = "";
    destination = "";
    startingLoc = "";

}

void METUMaps::UpdateLocation(const std::string& name)
{
    if (!inJourney) {
        PrintNotInJourney();
        return;
    }
    if (name == destination) {
        currentLoc = destination;
        PrintJourneyCompleted();
        return;
    }

    std::string key = GenerateKey(name, destination);
    std::vector<int> new_route;
    if (cachedPaths.Find(new_route, key)) {
        PrintCachedLocationFound(name, destination);
        currentRoute = new_route;
        currentLoc = name;
        return;
    }

    PrintCalculatingRoutes(name, destination);
    std::vector<std::vector<int> > path_list;
    try
    {
        map.MultipleShortPaths(path_list, name, destination, potentialPathCount);

    }
    catch(std::runtime_error &e)
    {
        PrintLocationNotFound();
        return;
    }    

    Caching(path_list);
    currentLoc = name;
    currentRoute = path_list[0];    
}

void METUMaps::Display()
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    if(!inJourney)
    {
        // We are not in journey, this function
        // shouldn't be called
        PrintNotInJourney();
        return;
    }

    int timeLeft = map.TotalWeightInBetween(currentRoute);

    // Print last known location
    std::cout << "Journey         : " << startingLoc << "->"
              << destination << "\n";
    std::cout << "Current Location: " << currentLoc << "\n";
    std::cout << "Time Left       : " << timeLeft << " minutes\n";
    std::cout << "Current Route   : ";
    map.PrintPath(currentRoute, true);

    std::cout.flush();
}