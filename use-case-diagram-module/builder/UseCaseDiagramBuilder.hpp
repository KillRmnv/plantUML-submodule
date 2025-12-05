#ifndef C974FD10_B617_4AC4_9E70_53C4B4560763
#define C974FD10_B617_4AC4_9E70_53C4B4560763
#pragma once 
#include <sc-memory/sc_addr.hpp>
#include <string>

#include "common/PackageDiagramBuilder.hpp"
#include "common/DiagramBuilder.hpp"

#include <unordered_map>
#include <unordered_set>

using namespace std;


# define SC_ADDR_LOCAL_TO_INT(addr) (sc_uint32)(((addr).seg << 16) | ((addr).offset & 0xffff))


struct ScAddrTripleHash
{
    size_t operator()(std::tuple<ScAddr,ScAddr,ScAddr> const & t) const
    {
        auto const& [a,b,c] = t;
        ScAddrHashFunc h;

        size_t r = 0;
        r ^= h(a) + 0x9e3779b97f4a7c15ULL + (r << 6) + (r >> 2);
        r ^= h(b) + 0x9e3779b97f4a7c15ULL + (r << 6) + (r >> 2);
        r ^= h(c) + 0x9e3779b97f4a7c15ULL + (r << 6) + (r >> 2);
        return r;
    }
};

struct ScAddrTripleEq
{
    bool operator()(std::tuple<ScAddr,ScAddr,ScAddr> const & x,
                    std::tuple<ScAddr,ScAddr,ScAddr> const & y) const
    {
        return  std::get<0>(x) == std::get<0>(y) &&
                std::get<1>(x) == std::get<1>(y) &&
                std::get<2>(x) == std::get<2>(y);
    }
};


class UseCaseDiagramBuilder : public PackageDiagramBuilder
{
public:
    UseCaseDiagramBuilder(ScMemoryContext * context, utils::ScLogger * logger);

    void ProcessNode(ScAddr Node,ScAddr package) override;
    void ProcessEdgesByNode(ScAddr Node,ScAddr package) override;
    void ProcessAdjacentNodes(ScAddr Node,ScAddr package) override;
    std::shared_ptr<ScAddrSet> GetAllPackages(ScAddr diagram) override;
    std::shared_ptr<ScAddrSet> GetUsedNodes(ScAddr diagram) override;
    std::string GetResultString() override;
    void ProcessPackage(ScAddr package) override;
    bool PackageCheck(ScAddr package,ScAddr parent) override;

private:
    void ProcessEdge(ScAddr edge,ScAddr relation,ScAddr package);
    std::string trim(const std::string &s);
    std::string trim_spaces(std::string str);

    std::unordered_map<int,
        std::unordered_set<
            std::tuple<ScAddr, ScAddr, ScAddr>,
            ScAddrTripleHash,
            ScAddrTripleEq
        >
    > structuresByLevel;

    ScAddrToValueUnorderedMap<std::tuple<ScAddr, ScAddr, ScAddr>> addrMap;
    
    int level = 0;
    std::string entitiesInCurrentPackage;
    std::string entities;
    std::string relations;
    std::string preamble;
    ScAddrToValueUnorderedMap<std::string> nameByNode;
    ScAddrToValueUnorderedMap<std::string> nameByStruct;
    ScAddrToValueUnorderedMap<ScAddrSet> actorsToActions;
};


#endif /* C974FD10_B617_4AC4_9E70_53C4B4560763 */
