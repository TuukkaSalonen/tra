#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <functional>
#include <exception>
#include <map>
#include <unordered_set>
#include <set>
#include <math.h>
// Types for IDs
using AffiliationID = std::string;
using PublicationID = unsigned long long int;
using Name = std::string;
using Year = unsigned short int;
using Weight = int;
using Distance = int;

// Return values for cases where required thing was not found
AffiliationID const NO_AFFILIATION = "---";
PublicationID const NO_PUBLICATION = -1;
Name const NO_NAME = "!NO_NAME!";
Year const NO_YEAR = -1;
Weight const NO_WEIGHT = -1;

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};


// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    /*
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
    */
    // TODO: REMOVE SQRT, UNNECESSARY
    Distance dist1 = c1.x*c1.x+c1.y*c1.y;
    Distance dist2 = c2.x*c2.x+c2.y*c2.y;
    if (dist1 < dist2){return true;}
    if (dist1 > dist2){return false;}
    if (c1.y<c2.y){return true;}
    else if (c1.y>c2.y){return false;}
    else {return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Return value for cases where Distance is unknown
Distance const NO_DISTANCE = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    NotImplemented() : msg_{} {}
    explicit NotImplemented(std::string const& msg) : msg_{msg + " not implemented"} {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};

struct Publication{
    PublicationID id;
    Name title;
    Year releaseYear;
    std::vector<AffiliationID> affiliations = {};
    std::vector<PublicationID> references = {};
    std::shared_ptr<Publication> parent = nullptr;
};

struct Affiliation{
    AffiliationID affiliationid;
    const Name name;
    Coord coord;
    std::vector<PublicationID> publications = {};
};

struct Affiliations{
    std::unordered_map<AffiliationID, Affiliation*> allAffiliations;
    unsigned int size = 0;
    std::map<Coord, AffiliationID> coordIDPair;
    void clearAffiliations(){
        allAffiliations.clear();
    }
};


// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: = O(1)
    // Short rationale for estimate: Return value of struct
    unsigned int get_affiliation_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Clears n elements
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate: N insertions to a vector
    std::vector<AffiliationID> get_all_affiliations();

    // Estimate of performance: O(n)
    // Short rationale for estimate: N insertions to a map
    bool add_affiliation(AffiliationID id, Name const& name, Coord xy);

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: Lookup time of unordered map
    Name get_affiliation_name(AffiliationID id);

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: Average lookup time of unordered map
    Coord get_affiliation_coord(AffiliationID id);


    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate: N insertions to a vector
    std::vector<AffiliationID> get_affiliations_alphabetically();

    // Estimate of performance: O(n)
    // Short rationale for estimate: N insertions to a vector
    std::vector<AffiliationID> get_affiliations_distance_increasing();

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: Average map lookup time
    AffiliationID find_affiliation_with_coord(Coord xy);

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: map insertion and lookup
    bool change_affiliation_coord(AffiliationID id, Coord newcoord);


    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate: log(n) for map insert and n for vector insert.
    bool add_publication(PublicationID id, Name const& name, Year year, const std::vector<AffiliationID> & affiliations);

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: Vector insertion n times
    std::vector<PublicationID> all_publications();

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: Average map lookup time
    Name get_publication_name(PublicationID id);

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: Average map lookup time
    Year get_publication_year(PublicationID id);

    // Estimate of performance: O(nlog(n))
    // Short rationale for estimate: Average map lookup time, n times
    std::vector<AffiliationID> get_affiliations(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Vector insertion time
    bool add_reference(PublicationID id, PublicationID parentid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Average map lookup time, n times
    std::vector<PublicationID> get_direct_references(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Average time of vector insert
    bool add_affiliation_to_publication(AffiliationID affiliationid, PublicationID publicationid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: N publications returned
    std::vector<PublicationID> get_publications(AffiliationID id);

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: Average map lookup time
    PublicationID get_parent(PublicationID id);

    // Estimate of performance: O(nlog(n))
    // Short rationale for estimate: Average time for sorting a vector
    std::vector<std::pair<Year, PublicationID>> get_publications_after(AffiliationID affiliationid, Year year);

    // Estimate of performance: O(nlog(n))
    // Short rationale for estimate: map lookup n times
    std::vector<PublicationID> get_referenced_by_chain(PublicationID id);


    // Non-compulsory operations

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<PublicationID> get_all_references(PublicationID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<AffiliationID> get_affiliations_closest_to(Coord xy);

    // Estimate of performance:
    // Short rationale for estimate:
    bool remove_affiliation(AffiliationID id);

    // Estimate of performance:
    // Short rationale for estimate:
    PublicationID get_closest_common_parent(PublicationID id1, PublicationID id2);

    // Estimate of performance:
    // Short rationale for estimate:
    bool remove_publication(PublicationID publicationid);


private:
    Affiliations affiliationStruct;
    std::unordered_map<PublicationID, Publication> allPublications;
    bool findPublication(PublicationID);
    std::map<Name, AffiliationID> nameMap;
    // IDs alphabetically
    std::vector<AffiliationID> IDvector;
    std::vector<AffiliationID> sortedAffiliations;
    bool affiliationExists(AffiliationID);
    std::unordered_map<PublicationID, Year> publicationYearMap;
    std::vector<PublicationID> referencesRecursively(std::vector<PublicationID> &publications, PublicationID id);

};
#endif // DATASTRUCTURES_HH
