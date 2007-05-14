#include <sys/mman.h>
#include "AddressSpace.h"
#include "nanassert.h"

void AddressSpace::SegmentDesc::save(ChkWriter &out) const{
  out << "Addr " << addr << " Len " << len;
  out << "R" << canRead;
  out << "W" << canWrite;
  out << "X" << canExec;
  out << "S" << shared;
  out << "F" << fileMap;
  out << "G" << autoGrow;
  out << "D" << growDown;
  out << endl;
}
ChkReader &AddressSpace::SegmentDesc::operator=(ChkReader &in){
  in >> "Addr " >> addr >> " Len " >> len;
  in >> "R" >> canRead;
  in >> "W" >> canWrite;
  in >> "X" >> canExec;
  in >> "S" >> shared;
  in >> "F" >> fileMap;
  in >> "G" >> autoGrow;
  in >> "D" >> growDown;  
  in >> endl;
  return in;
}

AddressSpace::FrameDesc::FrameDesc() : GCObject(){
  memset(data,0,AddrSpacPageSize);
}
AddressSpace::FrameDesc::FrameDesc(FrameDesc &src) : GCObject(){
  memcpy(data,src.data,AddrSpacPageSize);
}
AddressSpace::FrameDesc::~FrameDesc(){
  memset(data,0xCC,AddrSpacPageSize);
}
void AddressSpace::FrameDesc::save(ChkWriter &out) const{
  out.write(reinterpret_cast<const char *>(data),AddrSpacPageSize);
  out <<endl;
}
AddressSpace::FrameDesc::FrameDesc(ChkReader &in){
  in.read(reinterpret_cast<char *>(data),AddrSpacPageSize);
  in >> endl;
}
AddressSpace::PageDesc::PageDesc(void)
  : frame(0),
    insts(0),
    mapCount(0),
    canRead(0),
    canWrite(0),
    canExec(0),
    shared(false),
    copyOnWrite(false){
}
AddressSpace::PageDesc::PageDesc &AddressSpace::PageDesc::operator=(PageDesc &src){
  shared=src.shared;
  frame=src.frame;
  if(frame&&(!shared)){
    copyOnWrite=true;
    src.copyOnWrite=true;
  }
  mapCount=src.mapCount;
  canRead =src.canRead;
  canWrite=src.canWrite;
  canExec =src.canExec;
  insts=0;
  return *this;
}
void AddressSpace::PageDesc::save(ChkWriter &out) const{
  out << "S" << shared << " C" << copyOnWrite << " ";
  out.writeobj(getFrame());
}
ChkReader &AddressSpace::PageDesc::operator=(ChkReader &in){
  insts=0;
  mapCount=0;
  canRead=0;
  canWrite=0;
  canExec=0;
  in >> "S" >> shared >> " C" >> copyOnWrite >> " ";
  frame=in.readobj<FrameDesc>();
  return in;
}

AddressSpace::AddressSpace(void) :
  GCObject(),
  brkBase(0)
{
#if (defined SPLIT_PAGE_TABLE)
#else
  I(sizeof(pageMap)==AddrSpacPageNumCount*sizeof(PageDesc));
#endif
}  

AddressSpace::AddressSpace(AddressSpace &src) :
  GCObject(),
  segmentMap(src.segmentMap),
  brkBase(src.brkBase),
  funcAddrToName(src.funcAddrToName)
{
  // Copy page map and pages
#if (defined SPLIT_PAGE_TABLE)
  I(0);
#else
  I(sizeof(pageMap)==AddrSpacPageNumCount*sizeof(PageDesc));
  for(size_t i=0;i<AddrSpacPageNumCount;i++){
    PageDesc &srcPage=src.pageMap[i];
    PageDesc &dstPage=pageMap[i];
    dstPage=srcPage;
  }
#endif
  // Copy function name mappings
  for(AddrToNameMap::iterator addrIt=funcAddrToName.begin();
      addrIt!=funcAddrToName.end();addrIt++){
    addrIt->second=strdup(addrIt->second);
  }
}

void AddressSpace::clear(bool isExec){
  // Clear function name mappings
  while(!funcAddrToName.empty()){
    free(funcAddrToName.begin()->second);
    funcAddrToName.erase(funcAddrToName.begin());
  }
  // Remove all memory segments
  VAddr lastAddr=segmentMap.begin()->second.addr+segmentMap.begin()->second.len;
  while(!segmentMap.empty()){
    SegmentDesc &mySeg=segmentMap.begin()->second;
    I(mySeg.addr+mySeg.len<=lastAddr);
    I(mySeg.len>0);
    I(mySeg.addr==segmentMap.begin()->first);
    lastAddr=mySeg.addr;
    for(size_t pageNum=mySeg.pageNumLb();pageNum<mySeg.pageNumUb();pageNum++)
      getPageDesc(pageNum).unmap(mySeg.canRead,mySeg.canWrite,mySeg.canExec);
    segmentMap.erase(mySeg.addr);
  }
  // Remove all pages
#if (defined SPLIT_PAGE_TABLE)
  for(size_t rootPageNum=0;rootPageNum<AddrSpacRootSize;rootPageNum++){
    PageMapLeaf pageMapLeaf=pageMapRoot[rootNum];
    if(pageMapLeaf){
#if (defined DEBUG)
      for(size_t leafPageNum=0;leafPageNum<AddrSpacLeafSize;leafPageNum++)
	I((!pageMapLeaf[leafPageNum].mapCount)&&(!pageMapLeaf[leafPageNum].insts)&&(!pageMapLeaf[leafPageNum].frame));
#endif
      delete [] pageMapLeaf;
    }
  }
#else
#if (defined DEBUG)
  for(size_t pageNum=0;pageNum<AddrSpacPageNumCount;pageNum++)
    I((!pageMap[pageNum].mapCount)&&(!pageMap[pageNum].insts)&&(!pageMap[pageNum].frame));
#endif
#endif
}

AddressSpace::~AddressSpace(void){
  printf("Destroying an address space\n");
  clear(false);
}

void AddressSpace::save(ChkWriter &out) const{
  out << "BrkBase " << brkBase <<endl;
  // Page dump, dumps only pages that have any physical mapping
  for(size_t _pageNum=0;_pageNum<AddrSpacPageNumCount;_pageNum++){
#if (defined SPLIT_PAGE_TABLE)
    size_t _rootNum=(_pageNum>>AddrSpacLeafBits);
    PageMapLeaf pageMap=pageMapRoot[_rootNum];
    if(!pageMap){
      _pageNum+=(AddrSpacLeafSize-1);
      continue;
    }
#endif
    if(!pageMap[_pageNum].frame)
      continue;
    out << _pageNum << " ";
    pageMap[_pageNum].save(out);
  }
  // Page number zero signals end of page dump
  out << 0 << endl;
  // Segment dump
  out << "Segments " << segmentMap.size() << endl;
  for(SegmentMap::const_iterator segIt=segmentMap.begin();segIt!=segmentMap.end();segIt++)
    segIt->second.save(out);
  // Dump function name mappings
  out << "FuncNames " << funcAddrToName.size() << endl;
  for(AddrToNameMap::const_iterator addrIt=funcAddrToName.begin();
      addrIt!=funcAddrToName.end();addrIt++)
    out << addrIt->first << " -> " << strlen(addrIt->second) << " " << addrIt->second << endl;
}

AddressSpace::AddressSpace(ChkReader &in){
  in >> "BrkBase " >> brkBase >> endl;
  while(true){
    size_t _pageNum;
    in >> _pageNum;
    if(!_pageNum)
      break;
    in >> " ";
    getPageDesc(_pageNum)=in;
  }
  in >> endl;
  size_t _segCount;
  in >> "Segments " >> _segCount >> endl;
  for(size_t i=0;i<_segCount;i++){
    SegmentDesc seg;
    seg=in;
    newSegment(seg.addr,seg.len,seg.canRead,seg.canWrite,seg.canExec,seg.shared,seg.fileMap);
    setGrowth(seg.addr,seg.autoGrow,seg.growDown);
  }
  // Load function name mappings
  size_t _funcAddrToName;
  in >> "FuncNames " >> _funcAddrToName >> endl;
  for(size_t i=0;i<_funcAddrToName;i++){
    VAddr _addr; size_t _strlen;
    in >> _addr >> " -> " >> _strlen >> " ";
    char buf[_strlen+1];
    in >> buf >> endl;
    addFuncName(buf,_addr);
  }
}

// Add a new function name-address mapping
void AddressSpace::addFuncName(const char *name, VAddr addr){
  char *myName=strdup(name);
  I(myName);
  funcAddrToName.insert(AddrToNameMap::value_type(addr,myName));
}

// Return name of the function with given entry point
const char *AddressSpace::getFuncName(VAddr addr) const{
  AddrToNameMap::const_iterator nameIt=funcAddrToName.find(addr);
  if(nameIt->first!=addr)
    return 0;
  return nameIt->second;
}


// Given a code address, return where the function begins (best guess)
VAddr AddressSpace::getFuncAddr(VAddr addr) const{
  AddrToNameMap::const_iterator nameIt=funcAddrToName.upper_bound(addr);
  if(nameIt==funcAddrToName.begin())
    return 0;
  nameIt--;
  return nameIt->first;
}

// Given a code address, return the function size (best guess)
size_t AddressSpace::getFuncSize(VAddr addr) const{
  return 0;
}

// Print name(s) of function(s) with given entry point
void AddressSpace::printFuncName(VAddr addr){
  AddrToNameMap::iterator nameBegIt=funcAddrToName.lower_bound(addr);
  AddrToNameMap::iterator nameEndIt=funcAddrToName.upper_bound(addr);
  while(nameBegIt!=nameEndIt){
    printf("%s",nameBegIt->second);
    nameBegIt++;
    if(nameBegIt!=nameEndIt)
      printf(", ");
  }
}

VAddr AddressSpace::newSegmentAddr(size_t len){
  VAddr retVal=alignDown((VAddr)-1,AddrSpacPageSize);
  retVal=alignDown(retVal-len,AddrSpacPageSize);
  for(SegmentMap::const_iterator segIt=segmentMap.begin();segIt!=segmentMap.end();segIt++){
    const SegmentDesc &curSeg=segIt->second;
    if(retVal>=curSeg.addr+curSeg.len)
      break;
    if(alignUp(len+AddrSpacPageSize,AddrSpacPageSize)>curSeg.addr){
      retVal=0;
    }else{
      retVal=alignDown(curSeg.addr-len,AddrSpacPageSize);
    }
  }
  return retVal;
}
void AddressSpace::splitSegment(VAddr pivot){
  // Find segment that begins below pivot - this will be the one to split
  SegmentMap::iterator segIt=segmentMap.upper_bound(pivot);
  // Return if no such segment
  if(segIt==segmentMap.end())
    return;
  SegmentDesc &oldSeg=segIt->second;
  // If segment ends at or below pivot, no split needed
  if(oldSeg.addr+oldSeg.len<=pivot)
    return;
  SegmentDesc &newSeg=segmentMap[pivot];
  newSeg=oldSeg;
  newSeg.addr=pivot;
  newSeg.len=oldSeg.addr+oldSeg.len-newSeg.addr;
  oldSeg.len=oldSeg.len-newSeg.len;
  I((newSeg.pageNumLb()+1==oldSeg.pageNumUb())||(newSeg.pageNumLb()==oldSeg.pageNumUb()));
  if(newSeg.pageNumLb()!=oldSeg.pageNumUb())
    getPageDesc(newSeg.pageNumLb()).map(newSeg.canRead,newSeg.canWrite,newSeg.canExec);
}
void AddressSpace::growSegmentDown(VAddr oldaddr, VAddr newaddr){
  I(segmentMap.find(oldaddr)!=segmentMap.end());
  I(newaddr<oldaddr);
  SegmentDesc &oldSeg=segmentMap[oldaddr];
  I(isNoSegment(newaddr,oldaddr-newaddr));
  SegmentDesc &newSeg=segmentMap[newaddr];
  newSeg=oldSeg;
  newSeg.addr=newaddr;
  newSeg.len=oldaddr+oldSeg.len-newaddr;
  I(newSeg.pageNumUb()==oldSeg.pageNumUb());
  for(size_t pageNum=newSeg.pageNumLb();pageNum<oldSeg.pageNumLb();pageNum++)
    getPageDesc(pageNum).map(newSeg.canRead,newSeg.canWrite,newSeg.canExec);
  segmentMap.erase(oldaddr);
}
void AddressSpace::resizeSegment(VAddr addr, size_t newlen){
  I(segmentMap.find(addr)!=segmentMap.end());
  I(newlen>0);
  SegmentDesc &mySeg=segmentMap[addr];
  I(mySeg.addr==addr);
  size_t oldPageNumUb=mySeg.pageNumUb();
  mySeg.len=newlen;
  size_t newPageNumUb=mySeg.pageNumUb();
  if(oldPageNumUb<newPageNumUb){
    for(size_t pageNum=oldPageNumUb;pageNum<newPageNumUb;pageNum++)
      getPageDesc(pageNum).map(mySeg.canRead,mySeg.canWrite,mySeg.canExec);
  }else if(oldPageNumUb>newPageNumUb){
    for(size_t pageNum=newPageNumUb;pageNum<oldPageNumUb;pageNum++)
      getPageDesc(pageNum).unmap(mySeg.canRead,mySeg.canWrite,mySeg.canExec);
  }
}
void AddressSpace::moveSegment(VAddr oldaddr, VAddr newaddr){
  I(segmentMap.find(oldaddr)!=segmentMap.end());
  SegmentDesc &oldSeg=segmentMap[oldaddr];
  I(isNoSegment(newaddr,oldSeg.len));
  SegmentDesc &newSeg=segmentMap[newaddr];
  newSeg=oldSeg;
  newSeg.addr=newaddr;
  size_t oldPg=oldSeg.pageNumLb();
  size_t newPg=newSeg.pageNumLb();
  while(oldPg<oldSeg.pageNumUb()){
    I(newPg<newSeg.pageNumUb());
    PageDesc &oldPageDesc=getPageDesc(oldPg);
    PageDesc &newPageDesc=getPageDesc(newPg);
    newPageDesc.frame=oldPageDesc.frame;
    oldPageDesc.frame=0;
    I(newPageDesc.mapCount==0);
    I(oldPageDesc.mapCount==1);
    newPageDesc.mapCount=oldPageDesc.mapCount;
    oldPageDesc.mapCount=0;
    I(newPageDesc.canRead==0);
    I(oldPageDesc.canRead==(oldSeg.canRead?1:0));
    newPageDesc.canRead=oldPageDesc.canRead;
    oldPageDesc.canRead=0;
    I(newPageDesc.canWrite==0);
    I(oldPageDesc.canWrite==(oldSeg.canWrite?1:0));
    newPageDesc.canWrite=oldPageDesc.canWrite;
    oldPageDesc.canWrite=0;
    I(!oldPageDesc.insts);
    I(oldPageDesc.canExec==0);
    I(newPageDesc.canExec==0);
    I(oldPageDesc.canExec==(oldSeg.canExec?1:0));
    newPageDesc.canExec=oldPageDesc.canExec;
    oldPageDesc.canExec=0;
    oldPg++;
    newPg++;
  }
  I(newPg==newSeg.pageNumUb());
  segmentMap.erase(oldaddr);
}
void AddressSpace::deleteSegment(VAddr addr, size_t len){
  splitSegment(addr);
  splitSegment(addr+len);
  // Find the segment that begins before the end of deletion region
  SegmentMap::iterator begIt=segmentMap.upper_bound(addr+len);
  // If nothing begins before the end of deletion, nothing to do
  if(begIt==segmentMap.end())
    return;
  // This segment should not go past the end of deletion (we did the split)
  I(begIt->second.addr+begIt->second.len<=addr+len);
  // Find the segment that begins before the start of deletion region
  SegmentMap::iterator endIt=segmentMap.upper_bound(addr);
  // This segment should not go past the start of deletion (we did the split)
  I((endIt==segmentMap.end())||(endIt->second.addr+endIt->second.len<=addr));
  for(SegmentMap::iterator segIt=begIt;segIt!=endIt;segIt++){
    SegmentDesc &seg=segIt->second;
    I((seg.addr>=addr)&&(seg.addr+seg.len<=addr+len));
    for(size_t pageNum=seg.pageNumLb();pageNum<seg.pageNumUb();pageNum++)
      getPageDesc(pageNum).unmap(seg.canRead,seg.canWrite,seg.canExec);
  }
  segmentMap.erase(begIt,endIt);
#if (defined DEBUG)
  for(SegmentMap::iterator segIt=segmentMap.begin();segIt!=segmentMap.end();segIt++){
    SegmentDesc &seg=segIt->second;    
    I((seg.addr+seg.len<=addr)||(seg.addr>=addr+len));
  }
#endif
//   while(true){
//     // Find the segment that begins before the end of deletion
//     SegmentMap::iterator segIt=segmentMap.upper_bound(addr+len);
//     // If no segment begins before end of deletion, no deletion is needed
//     if(segIt==segmentMap.end())
//       break;
//     SegmentDesc &oldSeg=segIt->second;
//     // If the segment end before the start of deletion, no deletion is needed
//     if(oldSeg.addr+oldSeg.len<=addr)
//       break;
//     // If the segment ends after the end of deletion, we must split it
//     if(oldSeg.addr+oldSeg.len>addr+len){
//       I(0);
//       SegmentDesc &newSeg=segmentMap[addr+len];
//       newSeg=oldSeg;
//       newSeg.addr=addr+len;
//       newSeg.len=oldSeg.addr+oldSeg.len-newSeg.addr;
//       oldSeg.len=oldSeg.len-newSeg.len;
//       if(newSeg.pageNumLb()<oldSeg.pageNumUb()){
// 	I(newSeg.pageNumLb()+1==oldSeg.pageNumUb());
// 	addPages(newSeg.pageNumLb(),newSeg.pageNumUb(),newSeg.canRead,newSeg.canWrite,newSeg.canExec);
//       }
//       continue;
//     }
//     // If we got here, the segment ends in the deletion region
//     // If it begins before the deletion region, we must truncate it
//     if(oldSeg.addr<addr){
//       size_t oldPageNumUb=oldSeg.pageNumUb();
//       oldSeg.len=addr-oldSeg.addr;
//       if(oldSeg.pageNumUb()<oldPageNumUb)
// 	for(size_t pageNum=oldSeg.pageNumUb();pageNum<oldPageNumUb;pageNum++)
// 	  getPageDesc(pageNum).unmap(oldSeg.canRead,oldSeg.canWrite,oldSeg.canExec);
//       continue;
//     }
//     // If we got here, the segment is within the deletion region and is deleted
//     I(oldSeg.addr>=addr);
//     I(oldSeg.addr+oldSeg.len<=addr+len);
//     I(oldSeg.pageNumLb()<=oldSeg.pageNumUb());
//     for(size_t pageNum=oldSeg.pageNumLb();pageNum<oldSeg.PageNumUb();pageNum++)
//       getPageDesc(pageNum).unmap(oldSeg.canRead,oldSeg.canWrite,oldSeg.canExec);
//     segmentMap.erase(segIt);
//   }
}
void AddressSpace::newSegment(VAddr addr, size_t len, bool canRead, bool canWrite, bool canExec, bool shared, bool fileMap){
  I(len>0);
  I(isNoSegment(addr,len));
  SegmentDesc &newSeg=segmentMap[addr];
  newSeg.addr=addr;
  newSeg.len=len;
  newSeg.canRead=canRead;
  newSeg.canWrite=canWrite;
  newSeg.canExec=canExec;
  newSeg.autoGrow=false;
  newSeg.growDown=false;
  I(!shared);
  newSeg.shared=shared;
  newSeg.fileMap=fileMap;
  I(!fileMap);
  for(size_t pageNum=newSeg.pageNumLb();pageNum<newSeg.pageNumUb();pageNum++)
    getPageDesc(pageNum).map(canRead,canWrite,canExec);
}
void AddressSpace::protectSegment(VAddr addr, size_t len, bool canRead, bool canWrite, bool canExec){
  splitSegment(addr);
  splitSegment(addr+len);
  I(isSegment(addr,len));
  SegmentDesc &mySeg=segmentMap[addr];
  I(mySeg.addr==addr);
  I(mySeg.len==len);
  for(size_t pageNum=mySeg.pageNumLb();pageNum<mySeg.pageNumUb();pageNum++){
    getPageDesc(pageNum).allow(canRead&&!mySeg.canRead,canWrite&&!mySeg.canWrite,canExec&&!mySeg.canExec);
    getPageDesc(pageNum).deny(mySeg.canRead&&!canRead,mySeg.canWrite&&!canWrite,mySeg.canExec&&!canExec);
  }
  mySeg.canRead=canRead;
  mySeg.canWrite=canWrite;
  mySeg.canExec=canExec;
}
