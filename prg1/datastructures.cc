#include "datastructures.hh"
#include <iostream>

#include <random>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <unordered_set>
#include <memory>
#include <math.h>
std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

//struct Affiliations affiliationStruct;

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Write any initialization you need here
    //Affiliations *affiliationStruct = new Affiliations();
}

Datastructures::~Datastructures()
{
    for(auto& i:affiliationStruct.allAffiliations){
        delete i.second;
        i.second = nullptr;
    }
}

bool Datastructures::findPublication(PublicationID id){
    // If a key is found, this will be false
    if(allPublications.find(id) == allPublications.end()){
        return false;
    }
     else{
        return true;
    }
}
bool Datastructures::affiliationExists(AffiliationID id){
    return affiliationStruct.allAffiliations.find(id)!=affiliationStruct.allAffiliations.end();
}

unsigned int Datastructures::get_affiliation_count()
{
    return affiliationStruct.size;
}

void Datastructures::clear_all()
{
    for(auto& i:affiliationStruct.allAffiliations){
        delete i.second;
        i.second = nullptr;
    }
    affiliationStruct.allAffiliations.clear();
    nameMap.clear();
    IDvector.clear();
    affiliationStruct.coordIDPair.clear();
    sortedAffiliations.clear();

    affiliationStruct.size=0;
    allPublications.clear();
}

std::vector<AffiliationID> Datastructures::get_all_affiliations()
{
    std::vector<AffiliationID> aha;
    aha.reserve(affiliationStruct.allAffiliations.size());
    for(const auto &i : affiliationStruct.allAffiliations){
        aha.push_back(i.first);
    }
    return aha;
}

bool Datastructures::add_affiliation(AffiliationID id, const Name &name, Coord xy)
{
    if(affiliationExists(id)){
        return false;
    }
    else{
        affiliationStruct.allAffiliations.insert({id, new Affiliation{id, name, xy}});
        nameMap.insert({name, id});
        affiliationStruct.coordIDPair.insert({xy, id});
        affiliationStruct.size+=1;
        return true;
    }
}

Name Datastructures::get_affiliation_name(AffiliationID id)
{
    if(affiliationExists(id)){
        return affiliationStruct.allAffiliations.at(id)->name;
    }
    return NO_NAME;
}

Coord Datastructures::get_affiliation_coord(AffiliationID id)
{
    if(affiliationExists(id)){
        return affiliationStruct.allAffiliations.at(id)->coord;
    }
    return NO_COORD;

}

std::vector<AffiliationID> Datastructures::get_affiliations_alphabetically()
{

    // Avoid reallocations by reserving the space
    if(IDvector.size()==0){
        IDvector.reserve(nameMap.size());
        for(const auto &i:nameMap){
            IDvector.push_back(i.second);
        }
        return IDvector;
    }
    return IDvector;
}

std::vector<AffiliationID> Datastructures::get_affiliations_distance_increasing()
{
    if (sortedAffiliations.size() != affiliationStruct.coordIDPair.size()) {
        sortedAffiliations.clear(); // Clear the vector if it's not the right size

        sortedAffiliations.reserve(affiliationStruct.coordIDPair.size());
        std::transform(affiliationStruct.coordIDPair.begin(),
                       affiliationStruct.coordIDPair.end(),
                       std::back_inserter(sortedAffiliations),
                       [](const auto& pair) {
                           return pair.second;
                       });
    }

    return sortedAffiliations;
}

AffiliationID Datastructures::find_affiliation_with_coord(Coord xy)
{
    if(affiliationStruct.coordIDPair.count(xy)==1){
        return affiliationStruct.coordIDPair.at(xy);
    }
    return NO_AFFILIATION;
}

bool Datastructures::change_affiliation_coord(AffiliationID id, Coord newcoord)
{
    if(affiliationExists(id)){
        if(affiliationStruct.allAffiliations.at(id)->coord==newcoord){
            return true;
        }
        auto key = affiliationStruct.allAffiliations.at(id)->coord;
        affiliationStruct.coordIDPair.erase(key);
        affiliationStruct.coordIDPair.insert({newcoord, id});
        affiliationStruct.allAffiliations.at(id)->coord = newcoord;
        sortedAffiliations.clear();
        return true;
    }
    return false;
}

bool Datastructures::add_publication(PublicationID id, const Name &name, Year year, const std::vector<AffiliationID> &affiliations)
{

    if((findPublication(id))){
        return false;
    }
    allPublications.insert({id, {id, name, year, affiliations}});
    publicationYearMap.insert({id, year});
    for(auto const &i : affiliations){
        affiliationStruct.allAffiliations.at(i)->publications.push_back(id);
    }

    return true;

}

std::vector<PublicationID> Datastructures::all_publications()
{
    std::vector<PublicationID> publicationIDs;

    for(auto &i: allPublications){
        publicationIDs.push_back(i.first);
    }
    return publicationIDs;
}

Name Datastructures::get_publication_name(PublicationID id)
{
    if(findPublication(id)){
        return allPublications.at(id).title;
    }
    return NO_NAME;
}

Year Datastructures::get_publication_year(PublicationID id)
{
    if(findPublication(id)){
        return allPublications.at(id).releaseYear;
    }
    return NO_YEAR;
}

std::vector<AffiliationID> Datastructures::get_affiliations(PublicationID id)
{
    if(findPublication(id)){
        return allPublications.at(id).affiliations;
    }
    std::vector<AffiliationID> ihaa = {NO_AFFILIATION};
    return ihaa;
}

bool Datastructures::add_reference(PublicationID id, PublicationID parentid)
{
    if(findPublication(id) && findPublication(parentid)){
        allPublications.at(parentid).references.push_back(id);
        allPublications.at(id).parent = std::make_shared<Publication>(allPublications.at(parentid));
        return true;
    }
    return false;
}

std::vector<PublicationID> Datastructures::get_direct_references(PublicationID id)
{
    if(findPublication(id)){
        return allPublications.at(id).references;
    }
    std::vector<PublicationID> hmm = {NO_PUBLICATION};
    return hmm;
}

bool Datastructures::add_affiliation_to_publication(AffiliationID affiliationid, PublicationID publicationid)
{
    if(findPublication(publicationid)){
        allPublications.at(publicationid).affiliations.push_back(affiliationid);
        affiliationStruct.allAffiliations.at(affiliationid)->publications.push_back(publicationid);
        return true;
    }
    return false;

}

std::vector<PublicationID> Datastructures::get_publications(AffiliationID id)
{
    if(affiliationExists(id)){
        return affiliationStruct.allAffiliations.at(id)->publications;
    }
    std::vector<PublicationID> aha = {NO_PUBLICATION};
    return aha;
}

PublicationID Datastructures::get_parent(PublicationID id)
{
    if(findPublication(id)){
        if(allPublications.at(id).parent!=nullptr){
              return allPublications.at(id).parent->id;
        }
    }
    return NO_PUBLICATION;
}
std::vector<std::pair<Year, PublicationID> > Datastructures::get_publications_after(AffiliationID affiliationid, Year year)
{
    // Check that the map contains the affiliationID
    if(affiliationExists(affiliationid)){
        std::vector<std::pair<Year, PublicationID> > p;

        for(auto i : affiliationStruct.allAffiliations.at(affiliationid)->publications){
            if(publicationYearMap.at(i)>=year){
                p.push_back({allPublications.at(i).releaseYear, allPublications.at(i).id});
            }
        }
        std::sort(p.begin(), p.end(), [](auto i, auto j){
            if(i.first!=j.first){
                return i.first<j.first;
            }
            else{
                return i.second<j.second;
            }
        });
        return p;
    }
    std::vector<std::pair<Year, PublicationID> > lul = {{NO_YEAR, NO_PUBLICATION}};
    return lul;
}

std::vector<PublicationID> Datastructures::get_referenced_by_chain(PublicationID id)
{
    if(findPublication(id)){
        std::vector<PublicationID> publi;

        while(id!=NO_PUBLICATION){
            id = get_parent(id);
            if(id!=NO_PUBLICATION){
                publi.push_back(id);
            }
        }
        return publi;
    }
    std::vector<PublicationID> publi1 = {NO_PUBLICATION};
    return publi1;
}

std::vector<PublicationID> Datastructures::referencesRecursively(std::vector<PublicationID> &publications, PublicationID id){
    id = allPublications.at(id).id;
    for(const auto &i: allPublications.at(id).references){
        if((allPublications.at(id).references.size()!=0)){
            id = i;
            publications.insert(publications.begin(), allPublications.at(i).references.begin(), allPublications.at(i).references.end());
            referencesRecursively(publications, id);
        }
    }
    return publications;
}
std::vector<PublicationID> Datastructures::get_all_references(PublicationID id)
{
    // USE RECURSION
    if(findPublication(id)){
        std::vector<PublicationID> references;
        references.reserve(allPublications.at(id).references.size());
        references.insert(references.begin(), allPublications.at(id).references.begin(), allPublications.at(id).references.end());
        referencesRecursively(references, id);
        return references;
    }
    std::vector<PublicationID> notFound = {NO_PUBLICATION};
    return notFound;
}
std::vector<AffiliationID> Datastructures::get_affiliations_closest_to(Coord xy)
{
    if(affiliationStruct.allAffiliations.size()>0){
        std::map<int, AffiliationID> IDs;

        // Since it 3 closes coordinates will be picked, we can iterate from [lower_bound-3, lower_bound+3]
        // because the coordinates have to be in that range (since they are sorted)
        auto it = affiliationStruct.coordIDPair.lower_bound(xy);

        auto itt = std::distance(affiliationStruct.coordIDPair.begin(), it);

        if(itt<3){
            while(it!=affiliationStruct.coordIDPair.begin()){
                --it;
            }
        }
        while(it!=affiliationStruct.coordIDPair.end()){
            int distance = sqrt(std::pow(it->first.x-xy.x,2) + std::pow(it->first.y-xy.y,2));
            if(IDs.size()<3){
                IDs.insert({distance, it->second});
            }
            else{
                for(auto itr = IDs.begin(); itr!=IDs.end();){
                    if(distance < itr->first){
                        IDs.erase(itr);
                        IDs.insert({distance, it->second});
                        break;
                    }
                    else{
                        ++itr;
                    }
                }
            }
            ++it;
        }
        std::vector<AffiliationID> okay;
        for(auto i: IDs){
            okay.push_back(i.second);
        }
        return okay;

    }
    else{
        std::vector<AffiliationID> okay;
        return okay;
    }
}

bool Datastructures::remove_affiliation(AffiliationID id)
{
    if(!(affiliationExists(id))){
        return false;
    }
    for(auto i : allPublications){
        auto idIter = std::find(i.second.affiliations.begin(), i.second.affiliations.end(), id);
        if(idIter != i.second.affiliations.end()){
            i.second.affiliations.erase(idIter);
            break;
        }
    }
    delete affiliationStruct.allAffiliations.at(id);
    affiliationStruct.allAffiliations.erase(id);

    if(std::find(IDvector.begin(), IDvector.end(), id)!=IDvector.end()){
        IDvector.erase(std::find(IDvector.begin(), IDvector.end(), id));
    }

    auto it = std::find(sortedAffiliations.begin(), sortedAffiliations.end(), id);
    if(it!=sortedAffiliations.end()){
        sortedAffiliations.erase(it);
    }
    for(auto i: affiliationStruct.coordIDPair){
        if(i.second==id){
            affiliationStruct.coordIDPair.erase(i.first);
            break;
        }
    }
    affiliationStruct.size-=1;
    return true;
}

PublicationID Datastructures::get_closest_common_parent(PublicationID id1, PublicationID id2)
{
    std::vector<PublicationID> id1Parents = get_referenced_by_chain(id1);
    std::vector<PublicationID> id2Parents = get_referenced_by_chain(id2);

    auto id1size = size_t(id1Parents.size());
    auto id2size = size_t(id2Parents.size());
    for(unsigned long long i = 0; i < id1size; i++){
        for(unsigned long long j = 0; j < id2size; j++){
            if((id1Parents.at(i)!=id1) && (id2Parents.at(j)!=id2) &&(id1Parents.at(i)==id2Parents.at(j))){
                return id1Parents.at(i);
            }
        }
    }
    return NO_PUBLICATION;
}




bool Datastructures::remove_publication(PublicationID publicationid)
{
    if(!(findPublication(publicationid))){
        return false;
    }
    for(auto i: allPublications){
        if(std::find(i.second.references.begin(), i.second.references.end(), publicationid)!=i.second.references.end()){
            i.second.references.erase(std::find(i.second.references.begin(), i.second.references.end(), publicationid));
        }

        if(i.second.parent!=nullptr){
            if(i.second.parent->id==publicationid){
                i.second.parent->id = NO_PUBLICATION;
            }
        }
    }
    if(publicationYearMap.find(publicationid)!=publicationYearMap.end()){
        publicationYearMap.erase(publicationid);
    }
    if(allPublications.find(publicationid)!=allPublications.end()){
        allPublications.erase(publicationid);
    }
    for(auto i : affiliationStruct.allAffiliations){
        if(std::find(i.second->publications.begin(), i.second->publications.end(), publicationid)!=i.second->publications.end()){
            i.second->publications.erase(std::find(i.second->publications.begin(), i.second->publications.end(), publicationid));
        }
    }


    return true;
}

