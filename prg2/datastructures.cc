// Datastructures.cc
//
// Student name: Tuukka Salonen

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <set>
#include <algorithm>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Write any initialization you need here
}

Datastructures::~Datastructures()
{
    clear_all();
}

// General affiliation search function from the datastructure.
Affiliation* Datastructures::is_affiliation(AffiliationID id) {
    auto search = affiliations.find(id);

    if (search == affiliations.end()) {
        return nullptr;
    }
    return search->second;
}
// General publication search function from the datastructure.
Publication* Datastructures::is_publication(PublicationID id) {
    auto search = publications.find(id);

    if (search == publications.end()) {
        return nullptr;
    }
    return search->second;
}

unsigned int Datastructures::get_affiliation_count()
{
    return affiliations.size();
}

void Datastructures::clear_all()
{
    for (auto& pub : publications) {
        delete pub.second;
    }

    for (auto& aff : affiliations) {
        delete aff.second;
    }
    affiliations.clear();
    publications.clear();
    connections.clear();
}

std::vector<AffiliationID> Datastructures::get_all_affiliations()
{
    std::vector<AffiliationID> all_affiliations;

    std::transform(affiliations.begin(), affiliations.end(), std::back_inserter(all_affiliations),
                   [](const std::pair<AffiliationID, Affiliation*>& pair) {return pair.first;}
    );

    return all_affiliations;
}

bool Datastructures::add_affiliation(AffiliationID id, const Name &name, Coord xy)
{
    // Worst case O(n)
    if (is_affiliation(id) != nullptr) {
        return false;
    }
    Affiliation* new_affiliation = new Affiliation;
    new_affiliation->id = id;
    new_affiliation->name = name;
    new_affiliation->pos = xy;
    affiliations.insert({id, new_affiliation});
    return true;
}

Name Datastructures::get_affiliation_name(AffiliationID id)
{
    // Worst case O(n)
    auto search = is_affiliation(id);

    if (search == nullptr) {
        return NO_NAME;
    }
    return search->name;
}

Coord Datastructures::get_affiliation_coord(AffiliationID id)
{
    // Worst case O(n)
    auto search = is_affiliation(id);

    if (search == nullptr) {
        return NO_COORD;
    }
    return search->pos;
}

std::vector<AffiliationID> Datastructures::get_affiliations_alphabetically()
{
    std::vector<Affiliation*> sortedAffiliations;
    sortedAffiliations.reserve(affiliations.size());

    for (const auto& pair : affiliations) {
        sortedAffiliations.push_back(pair.second);
    }
    std::sort(sortedAffiliations.begin(), sortedAffiliations.end(), [](Affiliation* aff1, Affiliation* aff2) {
        return aff1->name < aff2->name;
    });

    std::vector<AffiliationID> sortedIDs;
    sortedIDs.reserve(sortedAffiliations.size());

    std::transform(sortedAffiliations.begin(), sortedAffiliations.end(), std::back_inserter(sortedIDs), [](const Affiliation* affiliation) {
        return affiliation->id;
    });
    return sortedIDs;
}

// Helper function to compare distances
bool compare_distance(Affiliation* a, Affiliation* b) {
    auto dist1 = std::sqrt((a->pos.x*a->pos.x) + (a->pos.y*a->pos.y));
    auto dist2 = std::sqrt((b->pos.x*b->pos.x) + (b->pos.y*b->pos.y));

    if (dist1 == dist2) {
        return a->pos.y < b->pos.y;
    }
    return dist1 < dist2;
}

std::vector<AffiliationID> Datastructures::get_affiliations_distance_increasing()
{
    std::vector<Affiliation*> sortedAffiliations;

    for (const auto& pair : affiliations) {
        sortedAffiliations.push_back(pair.second);
    }
    std::sort(sortedAffiliations.begin(), sortedAffiliations.end(), compare_distance);

    std::vector<AffiliationID> sortedIDs;
    sortedIDs.reserve(affiliations.size());

    std::transform(sortedAffiliations.begin(), sortedAffiliations.end(), std::back_inserter(sortedIDs), [](const Affiliation* affiliation) {
        return affiliation->id;
    });
    return sortedIDs;
}

AffiliationID Datastructures::find_affiliation_with_coord(Coord xy)
{
    auto it = std::find_if(affiliations.begin(), affiliations.end(),
                           [&xy](const std::pair<AffiliationID, Affiliation*>& pair) {
        return xy == pair.second->pos; });

    if (it != affiliations.end()) {
        return it->first;
    }
    return NO_AFFILIATION;
}

bool Datastructures::change_affiliation_coord(AffiliationID id, Coord newcoord)
{
    // Worst case O(n)
    auto search = is_affiliation(id);

    if (search == nullptr) {
        return false;
    }
    search->pos = newcoord;
    return true;
}

bool Datastructures::add_publication(PublicationID id, const Name &name, Year year, const std::vector<AffiliationID> &affiliations)
{
    // Worst case O(n)
    if (is_publication(id) != nullptr) {
        return false;
    }
    Publication* new_publication = new Publication;
    new_publication->id = id;
    new_publication->title = name;
    new_publication->year = year;

    if (affiliations.size() != 0) {
        new_publication->related_affiliations = affiliations;
        for (auto& affId : affiliations) {
            Affiliation* aff = is_affiliation(affId);
            aff->publications.push_back(new_publication);
        }
        add_connections(affiliations);
    }
    publications.insert({id, new_publication});
    return true;
}

std::vector<PublicationID> Datastructures::all_publications()
{
    std::vector<PublicationID> all_publications;
    all_publications.reserve(publications.size());

    std::transform(publications.begin(), publications.end(), std::back_inserter(all_publications),
                   [](const std::pair<PublicationID, Publication*> pair) {
        return pair.first;
    });
    return all_publications;
}

Name Datastructures::get_publication_name(PublicationID id)
{
    // Worst case O(n)
    auto search = is_publication(id);

    if (search == nullptr) {
        return NO_NAME;
    }
    return search->title;
}

Year Datastructures::get_publication_year(PublicationID id)
{
    // Worst case O(n)
    auto search = is_publication(id);

    if (search == nullptr) {
        return NO_YEAR;
    }
    return search->year;
}

std::vector<AffiliationID> Datastructures::get_affiliations(PublicationID id)
{
    std::vector<AffiliationID> affs = {NO_AFFILIATION};

    // Worst case O(n)
    auto search = is_publication(id);

    if (search == nullptr) {
        return affs;
    }
    return search->related_affiliations;
}

bool Datastructures::add_reference(PublicationID id, PublicationID parentid)
{
    // Worst case O(n)
    auto search1 = is_publication(id);
    auto search2 = is_publication(parentid);

    if (search1 == nullptr || search2 == nullptr) {
        return false;
    }

    if (search1->referencer != nullptr) {
        return false;
    }
    search2->references.push_back(search1);
    search1->referencer = search2;
    return true;
}

std::vector<PublicationID> Datastructures::get_direct_references(PublicationID id)
{
    std::vector<PublicationID> references;

    // Worst case O(n)
    auto search = is_publication(id);

    if (search == nullptr) {
        return references.push_back(NO_PUBLICATION), references;
    }
    std::vector<Publication*> direct_references = search->references;
    references.reserve(direct_references.size());

    for (auto& ref : direct_references) {
        references.push_back(ref->id);
    }
    return references;
}

bool Datastructures::add_affiliation_to_publication(AffiliationID affiliationid, PublicationID publicationid)
{
    // Worst case O(n)
    auto search_pub = is_publication(publicationid);
    auto search_aff = is_affiliation(affiliationid);

    if (search_aff == nullptr || search_pub == nullptr) {
        return false;
    }
    for (auto& affiliation : search_pub->related_affiliations) {
        add_connection(affiliationid, affiliation);
    }
    search_pub->related_affiliations.push_back(search_aff->id);
    search_aff->publications.push_back(search_pub);
    return true;
}

std::vector<PublicationID> Datastructures::get_publications(AffiliationID id)
{
    std::vector<PublicationID> pubs;

    // Worst case O(n)
    auto search = is_affiliation(id);

    if (search == nullptr) {
        return pubs.push_back(NO_PUBLICATION), pubs;
    }

    for (auto& pub : search->publications) {
        pubs.push_back(pub->id);
    }
    return pubs;
}

PublicationID Datastructures::get_parent(PublicationID id)
{
    // Worst case O(n)
    auto search = is_publication(id);

    if (search == nullptr) {
        return NO_PUBLICATION;
    }
    if (search->referencer == nullptr) {
        return NO_PUBLICATION;
    }
    return search->referencer->id;
}

std::vector<std::pair<Year, PublicationID> > Datastructures::get_publications_after(AffiliationID affiliationid, Year year)
{
    // Worst case O(n)
    auto search  = is_affiliation(affiliationid);

    if (search == nullptr) {
        return {{NO_YEAR, NO_PUBLICATION}};
    }

    std::vector<std::pair<Year, PublicationID>> pubs;
    const std::vector<Publication*> related_publications = search->publications;

    for (const Publication* pub : related_publications) {
        if (pub->year >= year) {
            pubs.emplace_back(pub->year, pub->id);
        }
    }
    std::sort(pubs.begin(), pubs.end(), [](std::pair<Year, PublicationID> pair1, std::pair<Year, PublicationID> pair2){
        if (pair1.first == pair2.first) {
            return pair1.second < pair2.second;
        }
        return pair1.first < pair2.first;
    });

    return pubs;
}

std::vector<PublicationID> Datastructures::get_referenced_by_chain(PublicationID id)
{
    std::vector<PublicationID> referencers;

    // Worst case O(n)
    auto publication = is_publication(id);

    if (publication == nullptr) {
        return referencers.push_back(NO_PUBLICATION), referencers;
    }

    while (publication->referencer != nullptr) {
        referencers.push_back(publication->referencer->id);
        publication = publication->referencer;
    }
    return referencers;
}

// Recursive helper function for get_all_references
void insert_references(std::vector<PublicationID> &references, Publication* publication) {
    if (publication->references.empty()) {
        return;
    }
    else {
        for (auto& reference : publication->references) {
            references.push_back(reference->id);
            insert_references(references, reference);
        }
    }
}

std::vector<PublicationID> Datastructures::get_all_references(PublicationID id)
{
    std::vector<PublicationID> references;

    // Worst case O(n)
    auto search = is_publication(id);

    if (search == nullptr) {
        return references.push_back(NO_PUBLICATION), references;
    }
    insert_references(references, search);

    return references;
}

// Helper function to calculate the distance between points
double distance(const Coord& a, const Coord& b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

std::vector<AffiliationID> Datastructures::get_affiliations_closest_to(Coord xy)
{
    std::vector<std::pair<AffiliationID, double>> closest_affiliations;

    for (auto &aff : affiliations) {
        double dist = distance(aff.second->pos, xy);
        closest_affiliations.emplace_back(aff.first, dist);
    }

    if (closest_affiliations.empty()) {
        return {};
    }

    std::sort(closest_affiliations.begin(), closest_affiliations.end(),[this](std::pair<AffiliationID, double>& a, std::pair<AffiliationID, double>& b) {
        if (a.second == b.second) {
            return affiliations[a.first]->pos.y < affiliations[b.first]->pos.y;
        }
        return a.second < b.second;
    });

    std::vector<AffiliationID> result;

    int count = closest_affiliations.size();

    if (count >= 3) {
        count = 3;
    }
    for (int i = 0; i < count; ++i) {
        result.push_back(closest_affiliations[i].first);
    }
    return result;
}

bool Datastructures::remove_affiliation(AffiliationID id)
{
    // Worst case O(n)
    auto search = is_affiliation(id);

    if (search == nullptr) {
        return false;
    }
    std::vector<Publication*> related_publications = search->publications;

    for (auto& pub : related_publications) {
        auto& affs = pub->related_affiliations;
        affs.erase(std::remove(affs.begin(), affs.end(), id), affs.end());
    }
    affiliations.erase(search->id);
    delete search;
    return true;
}

PublicationID Datastructures::get_closest_common_parent(PublicationID id1, PublicationID id2)
{
    // Worst case O(n)
    auto pub1 = is_publication(id1);
    auto pub2 = is_publication(id2);

    std::set<PublicationID> pub1parents;
    std::vector<PublicationID> pub2parents;

    if (pub1 == nullptr || pub2 == nullptr) {
        return NO_PUBLICATION;
    }

    while (pub1->referencer != nullptr) {
        pub1 = pub1->referencer;
        pub1parents.insert(pub1->id);
    }

    while (pub2->referencer != nullptr) {
        pub2 = pub2->referencer;
        pub2parents.push_back(pub2->id);
    }

    for (size_t i = 0; i < pub2parents.size(); i++) {
        PublicationID current =  pub2parents.at(i);

        if (pub1parents.find(current) != pub1parents.end())  {
            return current;
        }
    }
    return NO_PUBLICATION;
}

bool Datastructures::remove_publication(PublicationID publicationid)
{
    // Worst case O(n)
    auto search = is_publication(publicationid);

    if (search == nullptr) {
        return false;
    }
    auto& pub_references = search->references;

    for (auto& reference : pub_references) {
        reference->referencer = nullptr;
    }
    auto& referencer = search->referencer;

    if (referencer != nullptr) {
        auto& parent_refs = referencer->references;
        parent_refs.erase(std::remove_if(parent_refs.begin(), parent_refs.end(), [publicationid](Publication* pub)
                          {return pub->id == publicationid; }));
    }
    auto& affiliations = search->related_affiliations;

    for (auto& id : affiliations) {
        Affiliation* aff = is_affiliation(id);
        aff->publications.erase(std::remove_if(aff->publications.begin(), aff->publications.end(), [publicationid](Publication* pub)
                                {return pub->id == publicationid; }));
    }

    publications.erase(publicationid);
    delete search;
    return true;
}

bool Datastructures::compare_connection_affs_inverted(const Connection& c1, const Connection& c2) {
    return (c1.aff1 == c2.aff2 && c1.aff2 == c2.aff1);
}

bool Datastructures::compare_connection_affs(const Connection& c1, const Connection& c2) {
    return (c1.aff1 == c2.aff1 && c1.aff2 == c2.aff2);
}

bool Datastructures::has_connection(AffiliationID aff1, AffiliationID aff2) {
    auto search1 = connections.find(aff1);
    if (search1 != connections.end()) {
        const auto& connections1 = search1->second;

        auto it = std::find_if(connections1.begin(), connections1.end(),
            [aff1, aff2, this](const Connection& connection) {
                return compare_connection_affs(connection, {aff1, aff2, 0}) ||
                       compare_connection_affs(connection, {aff2, aff1, 0});
            });
        return it != connections1.end();
    }
    return false;
}

void Datastructures::add_connection(AffiliationID aff1, AffiliationID aff2) {
    if (aff1 == aff2) {
        return;
    }
    auto [it1, inserted1] = connections.emplace(aff1, std::vector<Connection>{});
    auto [it2, inserted2] = connections.emplace(aff2, std::vector<Connection>{});

    Connection new_connection1{aff1, aff2, 1};
    Connection new_connection2{aff2, aff1, 1};

    auto connection1It = std::find_if(it1->second.begin(), it1->second.end(),
        [&new_connection1, this](const Connection& existing_connection) {
            return compare_connection_affs(existing_connection, new_connection1);
        });

    auto connection2It = std::find_if(it2->second.begin(), it2->second.end(),
        [&new_connection2, this](const Connection& existing_connection) {
            return compare_connection_affs(existing_connection, new_connection2);
        });

    if (connection1It != it1->second.end()) {
        connection1It->weight += 1;
    } else {
        it1->second.push_back(new_connection1);
    }

    if (connection2It != it2->second.end()) {
        connection2It->weight += 1;
    } else {
        it2->second.push_back(new_connection2);
    }
}

void Datastructures::add_connections(const std::vector<AffiliationID>& affiliations) {
    for (auto it1 = affiliations.begin(); it1 != affiliations.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != affiliations.end(); ++it2) {
            add_connection(*it1, *it2);
        }
    }
}

std::vector<Connection> Datastructures::get_connected_affiliations(AffiliationID id)
{
    std::vector<Connection> connected_affiliations;
    auto it = connections.find(id);
    if (it != connections.end()) {
        connected_affiliations = it->second;
    }
    return connected_affiliations;
}

Connection Datastructures::search_inverted_connection(Connection connection) {
    auto aff = connections.find(connection.aff2);

    if (aff != connections.end()) {
        auto it = std::find_if(aff->second.begin(), aff->second.end(),
            [&connection, this](const Connection& connectionInAff) {
                return compare_connection_affs_inverted(connectionInAff, connection);
            });
        if (it != aff->second.end()) {
            return *it;
        }
    }
    return connection;
}

std::vector<Connection> Datastructures::get_all_connections() {
    std::vector<Connection> all_connections;

    for (const auto& affiliationConnections : connections) {
        const auto& affconnections = affiliationConnections.second;
        for (const auto& newconnection : affconnections) {
            auto duplicateIt = std::find_if(all_connections.begin(), all_connections.end(),
                [&](const Connection& connection) {
                    return newconnection == connection || compare_connection_affs_inverted(newconnection, connection);
                });

            if (duplicateIt == all_connections.end()) {
                if (newconnection.aff1 < newconnection.aff2) {
                    all_connections.push_back(newconnection);
                } else {
                    all_connections.push_back(search_inverted_connection(newconnection));
                }
            }
        }
    }
    return all_connections;
}

void Datastructures::find_any_path(AffiliationID current, AffiliationID target,
                                   std::set<AffiliationID>& visited, Path& path) {
    visited.insert(current);

    for (const auto& connection : connections[current]) {
        AffiliationID next_affiliation = (connection.aff1 == current) ? connection.aff2 : connection.aff1;

        if (visited.find(next_affiliation) == visited.end()) {
            path.push_back(connection);

            if (next_affiliation == target) {
                visited.insert(next_affiliation);
                return;
            }
            find_any_path(next_affiliation, target, visited, path);

            if (!path.empty() && path.back() == connection) {
                path.pop_back();
            }
        }
    }
}

Path Datastructures::get_any_path(AffiliationID source, AffiliationID target) {
    auto search1 = is_affiliation(source);
    auto search2 = is_affiliation(target);
    Path path;

    if (search1 == nullptr || search2 == nullptr) {
        return path;
    }
    std::set<AffiliationID> visited;
    find_any_path(source, target, visited, path);
    return path;
}


