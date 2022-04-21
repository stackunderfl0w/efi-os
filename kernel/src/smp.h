#include "typedef.h"
#include "graphics.h"
typedef struct  {
    char signature[4];
    uint32_t configuration_table;
    uint8_t length; // In 16 bytes (e.g. 1 = 16 bytes, 2 = 32 bytes)
    uint8_t mp_specification_revision;
    uint8_t checksum; // This value should make all bytes in the table equal 0 when added together
    uint8_t default_configuration; // If this is not zero then configuration_table should be 
                                   // ignored and a default configuration should be loaded instead
    uint32_t features; // If bit 7 is then the IMCR is present and PIC mode is being used, otherwise 
                       // virtual wire mode is; all other bits are reserved
}__attribute__ ((packed))mp_floating_pointer_structure ;

typedef struct  {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed))RSDPDescriptor;
typedef struct  {
 RSDPDescriptor firstPart;
 
 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed))RSDPDescriptor20;

typedef struct {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
}__attribute__ ((packed))ACPISDTHeader;

/*typedef struct  {
  ACPISDTHeader h;
  uint32_t PointerToOtherSDT[(h.Length - sizeof(h)) / 4];
}__attribute__ ((packed))RSDT;*/

void find_mp_table(){
    uint64_t ptr=0x10000;
    while (true){
        if((*((uint32_t*)ptr))==0x5F504D5F){
            mp_floating_pointer_structure* mp=(mp_floating_pointer_structure*)(ptr);
            printf("found!:%x\n",ptr);
            printf("conf table: %u\n",(uint64_t)mp->configuration_table);
            printf("length:%u\n",(uint64_t)mp->length);
            printf("rev:%u\n",(uint64_t)mp->mp_specification_revision);
            printf("checksum:%u\n",(uint64_t)mp->checksum);
            printf("def conf:%u\n",(uint64_t)mp->default_configuration);
            printf("features:%u\n",(uint64_t)mp->features);
            for (int i = 0; i < 32; ++i){
                uint8_t tmp=*((char*)ptr+i);
                print(to_hstring_noformat(tmp));
                printchar(' ');

            }
        }
        ptr++;
    }
}
void init_smp(void* rsdp){
    RSDPDescriptor20* table=(RSDPDescriptor20*)rsdp;
    printf("Revision:%x\n",table->firstPart.Revision);
    printf("RsdtAddress:%x\n",table->firstPart.RsdtAddress);
    printf("XsdtAddress:%x\n",table->XsdtAddress);

    ACPISDTHeader* rsdt=(ACPISDTHeader*)table->firstPart.RsdtAddress;
    for (int i = 0; i < 4; ++i)
    {
        printchar(rsdt->Signature[i]);
    }
    printf("Revision:%x\n",rsdt->Revision);

}

/*ACPISDTHeader* find_ACPI_Header(void* RootSDT){
    ACPISDTHeader *rsdt = (ACPISDTHeader *) RootSDT;
    int entries = (rsdt->Length - sizeof(rsdt->h)) / 4;
 
    for (int i = 0; i < entries; i++)
    {
        ACPISDTHeader *h = (ACPISDTHeader *) rsdt->PointerToOtherSDT[i];
        if (!strncmp(h->Signature, "FACP", 4))
            return (void *) h;
    }
 
    // No FACP found
    return NULL;
}*/