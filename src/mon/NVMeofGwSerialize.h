/*
 * NVMeofGwSerialize.h
 *
 *  Created on: Dec 29, 2023
 */

#ifndef MON_NVMEOFGWSERIALIZE_H_
#define MON_NVMEOFGWSERIALIZE_H_
#define dout_context g_ceph_context
#define dout_subsys ceph_subsys_mgr
#undef dout_prefix
#define dout_prefix *_dout << "nvmeofgw " << __PRETTY_FUNCTION__ << " "

inline std::ostream& operator<<(std::ostream& os, const GW_STATES_PER_AGROUP_E value) {
    switch (value) {
        case GW_STATES_PER_AGROUP_E::GW_IDLE_STATE: os << "IDLE "; break;
        case GW_STATES_PER_AGROUP_E::GW_STANDBY_STATE: os << "STANDBY "; break;
        case GW_STATES_PER_AGROUP_E::GW_ACTIVE_STATE: os << "ACTIVE "; break;
        case GW_STATES_PER_AGROUP_E::GW_BLOCKED_AGROUP_OWNER: os << "BLOCKED_AGROUP_OWNER "; break;
        case GW_STATES_PER_AGROUP_E::GW_WAIT_FAILBACK_PREPARED: os << "WAIT_FAILBACK_PREPARED "; break;
        default: os << "Invalid " << (int)value << " ";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const GW_AVAILABILITY_E value) {
  switch (value) {

        case GW_AVAILABILITY_E::GW_CREATED: os << "CREATED"; break;
        case GW_AVAILABILITY_E::GW_AVAILABLE: os << "AVAILABLE"; break;
        case GW_AVAILABILITY_E::GW_UNAVAILABLE: os << "UNAVAILABLE"; break;

        default: os << "Invalid " << (int)value << " ";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const GW_STATE_T value) {
    os << "GW_STATE_T [ \n (gw-mon) sm_state ";
    for (int i = 0; i < MAX_SUPPORTED_ANA_GROUPS; i++) {
        os << value.sm_state[i] << ",";
    }
    os <<  "] \n (gw-mon)  failover peers ";
    for (int i = 0; i < MAX_SUPPORTED_ANA_GROUPS; i++) {
        os << value.failover_peer[i] << ",";
    }
    os << "] availability " << value.availability << " version " << value.version << "]";
    return os;
};

inline std::ostream& operator<<(std::ostream& os, const SM_STATE value) {
    os << "SM_STATE [ ";
    for (int i = 0; i < MAX_SUPPORTED_ANA_GROUPS; i++) os << value[i];
    os << "]";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NqnState value) {
    os << "Subsystem( nqn: " << value.nqn << ", ANAGrpId: " << value.opt_ana_gid << ", " << value.sm_state << " )";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const GROUP_KEY value) {
    os << "GROUP_KEY {" << value.first << "," << value.second << "}";
    return os;
};

inline std::ostream& operator<<(std::ostream& os, const GWMAP value) {
    os << "GWMAP ";
    for (auto& nqn_state: value) {
        os << " { nqn: " << nqn_state.first << " -> ";
        for (auto& gw_state: nqn_state.second) {
            os << "\n (gw-mon)  { gw_id: " << gw_state.first << " -> " <<  gw_state.second << "}";
        }
        os << "}";
    }
    return os;
};


inline std::ostream& operator<<(std::ostream& os, const NONCE_VECTOR_T value) {
    for (auto & nonces : value){
        os <<  nonces << " ";
    }
    return os;
}


inline std::ostream& operator<<(std::ostream& os, const GW_ANA_NONCE_MAP value) {
   if(value.size()) os << "\n";
   for( auto &nonce_map : value){
       os  << "(gw-mon)  " << "ana_grp: " << nonce_map.first  << " [ " << nonce_map.second << "] \n" ;
   }
   return os;
}


inline std::ostream& operator<<(std::ostream& os, const GW_CREATED_T value) {
    os << "GW_CREATED_T { ana_group_id " << value.ana_grp_id << " nonce_map " <<  value.nonce_map <<  " }";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const GW_CREATED_MAP value) {
     if(value.size()) os << "\n";
      for( auto &gw_created_map : value){
          os  << "(gw-mon)  " << "gw_id: " << gw_created_map.first  << " [ " << gw_created_map.second << "] \n" ;
      }
      return os;
}

inline std::ostream& operator<<(std::ostream& os, const NVMeofGwMap value) {
    os << "NVMeofGwMap [ Gmap: ";
    for (auto& group_state: value.Gmap) {
        os << " { " << group_state.first << " } -> { " <<  group_state.second << " }";
    }
    os << " ] \n(gw-mon) [ Created_gws: ";
    for (auto& group_gws: value.Created_gws) {
        os << "\n(gw-mon) { " << group_gws.first << " } -> { " << group_gws.second << " }";
    }
    os << "]";
    return os;
}

inline void encode(const GW_STATE_T& state, ceph::bufferlist &bl) {
    for(int i = 0; i <MAX_SUPPORTED_ANA_GROUPS; i ++){
        encode((int)(state.sm_state[i]), bl);
    }
    for(int i = 0; i <MAX_SUPPORTED_ANA_GROUPS; i ++){
        encode((state.failover_peer[i]), bl);
    }
    encode(state.optimized_ana_group_id, bl);
    encode((int)state.availability, bl);
    encode(state.version, bl);
}

inline  void decode(GW_STATE_T& state,  ceph::bufferlist::const_iterator& bl) {
    int sm_state;
    GW_ID_T peer_name;
    for(int i = 0; i <MAX_SUPPORTED_ANA_GROUPS; i ++){
        decode(sm_state, bl);
        state.sm_state[i] = (GW_STATES_PER_AGROUP_E)  sm_state;
    }
    for(int i = 0; i <MAX_SUPPORTED_ANA_GROUPS; i ++){
        decode(peer_name, bl);
        state.failover_peer[i] = peer_name;
    }
    decode(state.optimized_ana_group_id, bl);
    int avail;
    decode(avail, bl);
    state.availability = (GW_AVAILABILITY_E)avail;
    decode(state.version, bl);
}

inline  void encode(const GW_METADATA_T& state, ceph::bufferlist &bl) {
    for(int i = 0; i <MAX_SUPPORTED_ANA_GROUPS; i ++){
        int tick = state.anagrp_sm_tstamps[i];
        encode( tick, bl);
     }
}

inline  void decode(GW_METADATA_T& state,  ceph::bufferlist::const_iterator& bl) {
    for(int i = 0; i <MAX_SUPPORTED_ANA_GROUPS; i ++){
        int tick;
        decode(tick, bl);
        state.anagrp_sm_tstamps[i] = tick;
    }
}

inline void encode(const GW_ANA_NONCE_MAP& nonce_map,  ceph::bufferlist &bl) {
    encode(nonce_map.size(), bl);
    for (auto& ana_group_nonces : nonce_map) {
        encode(ana_group_nonces.first, bl); // ana group id
        encode (ana_group_nonces.second.size(), bl); // encode the vector size
        for (auto& nonce: ana_group_nonces.second) encode(nonce, bl);
    }
}

inline void decode(GW_ANA_NONCE_MAP& nonce_map, ceph::buffer::list::const_iterator &bl) {
    size_t map_size;
    ANA_GRP_ID_T ana_grp_id;
    size_t vector_size;
    std::string nonce;

    decode(map_size, bl);
    for(size_t i = 0; i<map_size; i++){
        decode(ana_grp_id, bl);
        decode(vector_size,bl);
        for(size_t j = 0; j < vector_size; j++){
            decode (nonce, bl);
            nonce_map[ana_grp_id].push_back(nonce);
        }
    }
}

inline void encode(const GW_CREATED_MAP& gws,  ceph::bufferlist &bl) {
    encode (gws.size(), bl); // number of gws in the group
    for(auto& gw : gws){
        encode(gw.first, bl);// GW_id
        encode(gw.second.ana_grp_id, bl); // GW owns this group-id
        encode(gw.second.nonce_map, bl);
    }
}

inline void decode(GW_CREATED_MAP& gws, ceph::buffer::list::const_iterator &bl) {
    gws.clear();
    size_t num_created_gws;
    decode(num_created_gws, bl);

    for(size_t i = 0; i<num_created_gws; i++){
        std::string gw_name;
        decode(gw_name, bl);
        ANA_GRP_ID_T ana_grp_id;
        decode(ana_grp_id, bl);
        GW_CREATED_T gw_created(ana_grp_id);
        decode(gw_created.nonce_map, bl);

        gws[gw_name] = gw_created;
    }
}

inline void encode(const std::map<GROUP_KEY, GW_CREATED_MAP>& created_gws,  ceph::bufferlist &bl) {
    encode (created_gws.size(), bl); // number of groups
    for (auto& group_gws: created_gws) {
        auto& group_key = group_gws.first;
        encode(group_key.first, bl); // pool
        encode(group_key.second, bl); // group

        auto& gws = group_gws.second;
        encode (gws, bl); // encode group gws
    }
}

inline void decode(std::map<GROUP_KEY, GW_CREATED_MAP>& created_gws, ceph::buffer::list::const_iterator &bl) {
    created_gws.clear();
    size_t ngroups;
    decode(ngroups, bl);
    for(size_t i = 0; i<ngroups; i++){
        std::string pool, group;
        decode(pool, bl);
        decode(group, bl);
        GW_CREATED_MAP cmap;
        decode(cmap, bl);
        created_gws[std::make_pair(pool, group)] = cmap;
    }
}

inline void encode(const SUBSYST_GWMAP& subsyst_gwmap,  ceph::bufferlist &bl) {
    encode(subsyst_gwmap.size(), bl);
    dout(0) << "number gateways: " << subsyst_gwmap.size() << dendl;
    for (auto& subsyst: subsyst_gwmap) {
        encode(subsyst.first, bl);
        dout(0) << "gateway id: " << subsyst.first << dendl;
        encode(subsyst.second, bl);
        dout(0) << "gateway state: " << subsyst.second << dendl;
    }
}

inline void decode(SUBSYST_GWMAP& subsyst_gwmap, ceph::buffer::list::const_iterator &bl) {
    subsyst_gwmap.clear();
    size_t num_gws;
    decode(num_gws, bl);
    dout(0) << "number gateways: " << num_gws << dendl;

    for (size_t i = 0; i < num_gws; i++) {
        GW_ID_T gw_id;
        decode(gw_id, bl);
        dout(0) << "gw_id: " << gw_id << dendl;
        GW_STATE_T gw_st;
        decode(gw_st, bl);
        dout(0) << "gw_st: " << gw_st << dendl;
        subsyst_gwmap[gw_id] = gw_st;
    }
}

inline void encode(const GWMAP& nqn_gws_states,  ceph::bufferlist &bl) {
    encode(nqn_gws_states.size(), bl); // number nqn
    for (auto& nqn_gw_state: nqn_gws_states) {
        encode(nqn_gw_state.first, bl);// nqn
        encode(nqn_gw_state.second, bl);// encode the full map of this nqn : std::map<GW_ID_T, GW_STATE_T>
    }
}

inline void decode(GWMAP& nqn_gws_states, ceph::buffer::list::const_iterator &bl) {
    nqn_gws_states.clear();
    size_t num_subsystems;
    decode(num_subsystems, bl);
    dout(0) << "size: " << num_subsystems << dendl;

    for (size_t i = 0; i < num_subsystems; i++) {
        std::string nqn;
        decode(nqn, bl);
        dout(0) << "nqn: " << nqn << dendl;
        SUBSYST_GWMAP gw_map;
        decode(gw_map, bl);
        dout(0) << "SUBSYST_GWMAP: " << gw_map << dendl;
        nqn_gws_states[nqn] = gw_map;
    }
}

inline void encode(const std::map<GROUP_KEY, GWMAP>& gmap,  ceph::bufferlist &bl) {
    encode (gmap.size(), bl); // number of groups
    dout(0) << "size: " << gmap.size() << dendl;
    for (auto& group_state: gmap) {
        auto& group_key = group_state.first;
        encode(group_key.first, bl); // pool
        dout(0) << "pool: " << group_key.first << dendl;
        encode(group_key.second, bl); // group
        dout(0) << "group: " << group_key.second << dendl;

        encode(group_state.second, bl);
        dout(0) << "GWMAP: " << group_state.second << dendl;
    }
}

inline void decode(std::map<GROUP_KEY, GWMAP>& gmap, ceph::buffer::list::const_iterator &bl) {
    gmap.clear();
    size_t ngroups;
    decode(ngroups, bl);
    dout(0) << "ngroups: " << ngroups << dendl;
    for(size_t i = 0; i<ngroups; i++){
        std::string pool, group;
        decode(pool, bl);
        decode(group, bl);
        dout(0) << "pool: " << pool << " group: " << group << dendl;
        GWMAP grp_map;
        decode(grp_map, bl);
        dout(0) << "GWMAP: " << grp_map << dendl;
        gmap[std::make_pair(pool, group)] = grp_map;
    }
}

inline void encode(const std::map<GROUP_KEY, GWMETADATA>& gmetadata,  ceph::bufferlist &bl) {
    encode (gmetadata.size(), bl); // number of groups
    for (auto& group_md: gmetadata) {
        auto& group_key = group_md.first;
        encode(group_key.first, bl); // pool
        encode(group_key.second, bl); // group

        encode(group_md.second, bl);
    }
}

inline void decode(std::map<GROUP_KEY, GWMETADATA>& gmetadata, ceph::buffer::list::const_iterator &bl) {
    gmetadata.clear();
    size_t ngroups;
    decode(ngroups, bl);
    for(size_t i = 0; i<ngroups; i++){
        std::string pool, group;
        decode(pool, bl);
        decode(group, bl);
        GWMETADATA gmd;
        decode(gmd, bl);
        gmetadata[std::make_pair(pool, group)] = gmd;
    }
}

inline void encode(const GWMETADATA& group_md,  ceph::bufferlist &bl) {
    encode (group_md.size(), bl); // number of groups
    for (auto& nqn_md: group_md) {
        encode(nqn_md.first, bl); // nqn
        encode(nqn_md.second, bl); //  map of this nqn
    }
}

inline void decode(GWMETADATA& md, ceph::buffer::list::const_iterator &bl) {
    size_t num_subsystems;
    decode(num_subsystems, bl);
    for (size_t i = 0; i < num_subsystems; i++) {
        std::string nqn;
        decode(nqn, bl);
        SUBSYST_GWMETA gw_meta;
        decode(gw_meta, bl);
        md[nqn] = gw_meta;
    }
}

#undef dout_subsys
#endif /* SRC_MON_NVMEOFGWSERIALIZEP_H_ */
