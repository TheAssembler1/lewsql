#include "dmanager.h"

DiskManager::DiskId DiskManager::dopen() {
   int fd = open();

   if()
}

void DiskManager::dclose(DiskManager::DiskId disk_id) {
  
}

void DiskManager::dwrite(DiskManager::DiskId, DiskManager::PageId page_id, std::vector<std::byte> page_buffer) {
  
}

std::vector<std::byte> dread(DiskManager::DiskId disk_id, DiskManager::PageId page_id) {
  
}

