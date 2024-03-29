// -*- C++ -*-

#ifndef QDP_MASTERMAP_H
#define QDP_MASTERMAP_H

namespace QDP {

  class MasterMap {
  public:
    static MasterMap& Instance();
    int registrate(const Map& map);
    // const multi1d<int>& getInnerSites(int bitmask) const;
    // const multi1d<int>& getFaceSites(int bitmask) const;
    int getIdInner(int bitmask) const;
    int getIdFace(int bitmask) const;
    int getCountInner(int bitmask) const;
    int getCountFace(int bitmask) const;

  private:
    void complement(multi1d<int>& out, const multi1d<int>& orig) const;
    void uniquify_list_inplace(multi1d<int>& out , const multi1d<int>& ll) const;

    MasterMap() {
      //QDP_info("MasterMap() reserving");
      powerSet.reserve(2048);
      powerSetC.reserve(2048);
      powerSet.resize(1);
      powerSet[0] = new multi1d<int>;
      idInner.resize(1);
      idFace.resize(1);

#if 0
      identityMap = new int[ Layout::sitesOnNode() ];
      for (int i = 0 ; i < Layout::sitesOnNode() ; i++ )
	identityMap[i]=i;

      idInner[0] = QDPCache::Instance().registrateOwnHostMem( Layout::sitesOnNode() * sizeof(int) , (void*)identityMap , NULL );
#endif

      ////QDPIO::cout << "powerSet[0] size = " << powerSet[0]->size() << "\n";
    }

    int * identityMap;
    std::vector<const Map*> vecPMap;
    std::vector< multi1d<int>* > powerSet; // Power set of roffsets
    std::vector< multi1d<int>* > powerSetC; // Power set of complements
    std::vector< int > idInner;
    std::vector< int > idFace;
  };

} // namespace QDP

#endif
