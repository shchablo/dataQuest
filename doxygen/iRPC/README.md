# README
In these documents presented the description of classes the clustering algorithm for iRPC implement for CMSSW.

## How to use
```	 
  // 1) Fill these containers from raw data.
  iRPCHitContainer hr, lr;
  
  // 2) Clustering for HR and LR separate.
  iRPCClusterContainer chr, clr;
  std::thread thr (&iRPCClusterizer::clustering, this, info.thrTimeHR(), std::ref(hhr), std::ref(chr));
  std::thread tlr (&iRPCClusterizer::clustering, this, info.thrTimeLR(), std::ref(hlr), std::ref(clr));
  
  thr.join(); tlr.join();
  hlr.clear(); hhr.clear();
  
  // 3) Association between HR and LR.
  iRPCClusterContainer clusters = association(info.isAND(), info.thrDeltaTimeMin(), info.thrDeltaTimeMax(), chr, clr);
  chr.clear(); clr.clear();
  
  // 4) Compute clusters parameters.
  for(auto cl = clusters.begin(); cl != clusters.end(); ++cl)
    cl->compute(std::ref(info));
```
## Any questions
PhD-student, Shchablo Konstantin <br> 
Institute of Nuclear physics of Lyon  <br>
(Bâtiment Paul Dirac 4, Rue Enrico Fermi 69622 Villeurbanne Cedex, France) <br>
shchablo@ipnl.in2p3.fr or shchablo@gmail.com 

## License
GNU Lesser General Public License v3.0
