#ifndef GEOMINTERFACE_H__
#define GEOMINTERFACE_H__


#if defined(SWMM_GEOMETRY)

#define GNODE_INFLOW(index)             Node[##index##].inflow
#define GNODE_LATFLOW(index)            Node[##index##].newLatFlow
#define GNODE_DEPTH(index)              Node[##index##].newDepth
#define GNODE_INVERT(index)             Node[##index##].invertElev
#define GNODE_INFLOW_OBJECT(index)      Node[##index##].extInflow
#define GNODE_MAXDEPTH(index)           Node[##index##].fullDepth
#define GNODE_VOLUME(index)             Node[##index##].newVolume
#define GNODE_CANFLOOD(index)           Node[##index##].canFlood
#define GNODE_INITDEPTH(index)          Node[##index##].initDepth
#define GNODE_TYPE(index)               Node[##index##].type
#define GNODE_OVERFLOW(index)           Node[##index##].overflow
#define GNODE_HASINFLOW(index)          (Node[##index##].extInflow != NULL)

#define GNODE_COUNT                     Nobjects[NODE]
#define GNODE_TYPECOUNT(index)          Nnodes[##index##]

#define GCURVE_VOLUME(nodeIndex, depth) node_getVolume(##nodeIndex##, ##depth##)
#define GTABLE_INIT(table)              table_init(&##table##)
#define GTABLE_TSLOOKUP(tableIndex, date)       table_tseriesLookup(&Tseries[##tableIndex##], ##date##, FALSE)
#define GTABLE_LOOKUP(tableIndex, value)        table_lookup(&##tableIndex##, ##value##)
#define GTABLE_VALIDATE(table)          table_validate(&##table##)
#define GTABLE_ADDENTRY(table, x, y)    table_addEntry(&##table##, x, y)

#define GSYS_NODEINFLOW(index)          NodeInflow[##index##]
#define GSYS_NODESTATS_FLOODING(index)  NodeStats[##index##].timeFlooded
#define GSYS_NODESTATS_NUMNODESFLOODING StepFlowTotals.floodingNumNodes

#define GLINK_ID(index)                 Link[##index##].ID
#define GLINK_USNODE(index)             Link[##index##].node1
#define GLINK_DSNODE(index)             Link[##index##].node2
#define GLINK_USOFFSET(index)           Link[##index##].offset1
#define GLINK_DSOFFSET(index)           Link[##index##].offset2
#define GLINK_MAXDEPTH(index)           Link[##index##].xsect.yFull
#define GLINK_LENGTH(index)             Conduit[Link[##index##].subIndex].length
#define GLINK_ROUGHNESS(index)          Conduit[Link[##index##].subIndex].roughness
#define GLINK_SLOPE(index)              Conduit[Link[##index##].subIndex].slope
#define GLINK_HASGEOM(index)            (Link[##index##].xsect.type != DUMMY)
#define GLINK_GEOMTYPE(index)           Link[##index##].xsect.type
#define GLINK_TYPE(index)               Link[##index##].type
#define GLINK_DSINVERT(index)           (GNODE_INVERT(GLINK_DSNODE(##index##)) + GLINK_DSOFFSET(##index##))
#define GLINK_USINVERT(index)           (GNODE_INVERT(GLINK_USNODE(##index##)) + GLINK_USOFFSET(##index##))

#define LINKTYPE_CONDUIT                CONDUIT
#define XSGEOM_CIRCULAR                 CIRCULAR

#define GLINK_COUNT                     Nobjects[LINK]

#define GLINK_FLOW(index)               Link[##index##].newFlow
#define GLINK_VOLUME(index)             Link[##index##].newVolume
#define GLINK_USDEPTH(index)            Node[Link[##index##].node1].newDepth // Link[##index##].newDepth
#define GLINK_DSDEPTH(index)            Node[Link[##index##].node2].newDepth // Link[##index##].oldDepth


#else

#include "geometry/geometry.h"

#define GNODE_INFLOW(index)             this->m_geometry->findNode(index)->nodeInflow
#define GNODE_LATFLOW(index)            this->m_geometry->findNode(index)->latInflow
#define GNODE_DEPTH(index)              this->m_geometry->findNode(index)->depth
#define GNODE_INVERT(index)             this->m_geometry->findNode(index)->invertElev
#define GNODE_INFLOW_OBJECT(index)      this->m_geometry->findNode(index)->extInflow //TODO:
#define GNODE_MAXDEPTH(index)           this->m_geometry->findNode(index)->maxDepth
#define GNODE_VOLUME(index)             this->m_geometry->findNode(index)->volume
#define GNODE_CANFLOOD(index)           this->m_geometry->findNode(index)->canFlood
#define GNODE_INITDEPTH(index)          this->m_geometry->findNode(index)->initDepth
#define GNODE_TYPE(index)               this->m_geometry->findNode(index)->nodeType
#define GNODE_OVERFLOW(index)           this->m_geometry->findNode(index)->overflow
#define GNODE_HASINFLOW(index)          (Node[##index##].extInflow != NULL)

#define GNODE_COUNT                     Nobjects[NODE]
#define GNODE_TYPECOUNT(index)          Nnodes[##index##]

#define GCURVE_VOLUME(nodeIndex, depth) node_getVolume(##nodeIndex##, ##depth##)
#define GTABLE_INIT(table)              table_init(&##table##)
#define GTABLE_TSLOOKUP(tableIndex, date)       table_tseriesLookup(&Tseries[##tableIndex##], ##date##, FALSE)
#define GTABLE_LOOKUP(tableIndex, value)        table_lookup(&##tableIndex##, ##value##)
#define GTABLE_VALIDATE(table)          table_validate(&##table##)
#define GTABLE_ADDENTRY(table, x, y)    table_addEntry(&##table##, x, y)

#define GSYS_NODEINFLOW(index)          NodeInflow[##index##]
#define GSYS_NODESTATS_FLOODING(index)  NodeStats[##index##].timeFlooded
#define GSYS_NODESTATS_NUMNODESFLOODING StepFlowTotals.floodingNumNodes

#define GLINK_ID(index)                 Link[##index##].ID
#define GLINK_USNODE(index)             Link[##index##].node1
#define GLINK_DSNODE(index)             Link[##index##].node2
#define GLINK_USOFFSET(index)           Link[##index##].offset1
#define GLINK_DSOFFSET(index)           Link[##index##].offset2
#define GLINK_MAXDEPTH(index)           Link[##index##].xsect.yFull
#define GLINK_LENGTH(index)             Conduit[Link[##index##].subIndex].length
#define GLINK_ROUGHNESS(index)          Conduit[Link[##index##].subIndex].roughness
#define GLINK_SLOPE(index)              Conduit[Link[##index##].subIndex].slope
#define GLINK_HASGEOM(index)            (Link[##index##].xsect.type != DUMMY)
#define GLINK_GEOMTYPE(index)           Link[##index##].xsect.type
#define GLINK_TYPE(index)               Link[##index##].type
#define GLINK_DSINVERT(index)           (GNODE_INVERT(GLINK_DSNODE(##index##)) + GLINK_DSOFFSET(##index##))
#define GLINK_USINVERT(index)           (GNODE_INVERT(GLINK_USNODE(##index##)) + GLINK_USOFFSET(##index##))

#define LINKTYPE_CONDUIT                CONDUIT
#define XSGEOM_CIRCULAR                 CIRCULAR

#define GLINK_COUNT                     Nobjects[LINK]

#define GLINK_FLOW(index)               Link[##index##].newFlow
#define GLINK_VOLUME(index)             Link[##index##].newVolume
#define GLINK_USDEPTH(index)            Node[Link[##index##].node1].newDepth // Link[##index##].newDepth
#define GLINK_DSDEPTH(index)            Node[Link[##index##].node2].newDepth // Link[##index##].oldDepth


#endif


#endif//GEOMINTERFACE_H__
