#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include "bitmap-font.h"
#include "strutil.h"
 
typedef struct {
	void* BaseAddress;
	UINT64 BufferSize;
	UINT32 Width;
	UINT32 Height;
	UINT32 PixelsPerScanLine;
} Framebuffer;

 
typedef struct {
	uint32_t magic;			/* magic bytes to identify PSF */
	uint32_t version;		/* zero */
	uint32_t headersize;	/* offset of bitmaps in file, 32 */
	uint32_t flags;			/* 0 if there's no unicode table */
	uint32_t numglyph;		/* number of glyphs */
	uint32_t bytesperglyph;	/* size of each glyph */
	uint32_t height;		/* height in pixels */
	uint32_t width;			/* width in pixels */
	CHAR8* font;
} PSF_font;

typedef struct {
	Framebuffer* buf;
	bitmap_font* font;
	EFI_MEMORY_DESCRIPTOR* mem_map;
	UINTN map_size;
	UINTN map_desc_size;
	void* rsdp;
}bootinfo;


EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
	EFI_LOADED_IMAGE *loaded_image = NULL;					/* image interface */
	EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;		/* image interface GUID */
	EFI_FILE_IO_INTERFACE *IOVolume;						/* file system interface */
	EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;	/* file system interface GUID */
	EFI_FILE_HANDLE Volume;									/* the volume's interface */
 
	/* get the loaded image protocol interface for our "image" */
	uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
	/* get the volume handle */
	return LibOpenRoot(loaded_image->DeviceHandle);
}

UINT64 FileSize(EFI_FILE_HANDLE FileHandle)
{
	UINT64 ret;
	EFI_FILE_INFO       *FileInfo;         /* file information structure */
	/* get the file's size */
	FileInfo = LibFileInfo(FileHandle);
	ret = FileInfo->FileSize;
	FreePool(FileInfo);
	return ret;
}

UINT8* Load_Text_File(EFI_FILE_HANDLE Volume, CHAR16* FileName){
	//FileName = L"resources\\config.txt";
	EFI_FILE_HANDLE     FileHandle;
 

	uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);


	UINT64              ReadSize = FileSize(FileHandle);

	UINT8               *Buffer = AllocatePool(ReadSize+1);
	Buffer[ReadSize]=0x00;

	uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &ReadSize, Buffer);
	uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
	Print(L"%a\n",Buffer);
	return Buffer;
}
UINT8* Load_File(EFI_FILE_HANDLE Volume, CHAR16* FileName, UINT64 *ReadSize){
	EFI_FILE_HANDLE     FileHandle;

	uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);


	*ReadSize = FileSize(FileHandle);

	UINT8               *Buffer = AllocatePool(*ReadSize);

	uefi_call_wrapper(FileHandle->Read, 3, FileHandle, ReadSize, Buffer);
	uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
	return Buffer;
}

Framebuffer framebuffer;
EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
Framebuffer* setup_graphics(uint32_t pref_width, uint32_t pref_height){
	
	EFI_STATUS status;

 
	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
	if(EFI_ERROR(status)){
		Print(L"Unable to locate GOP\n\r");
		return NULL;
	}
	else{
		Print(L"GOP located\n\r");
	}

	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
	UINTN SizeOfInfo, numModes, nativeMode;
	numModes=0;

	status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);
	// this is needed to get the current video mode
	if (status == EFI_NOT_STARTED)
		status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
	if(EFI_ERROR(status)) {
		Print(L"Unable to get native mode\n");
	} else {
		nativeMode = gop->Mode->Mode;
		numModes = gop->Mode->MaxMode;
	}

	int mode = -1;
	uint32_t greatest_dif=1000000;


	for (unsigned int i = 0; i < numModes; i++) {
		status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &info);
		Print(L"mode %03d width %d height %d format %x%s",
			i,
			info->HorizontalResolution,
			info->VerticalResolution,
			info->PixelFormat,
			i == nativeMode ? "(current)" : ""
			,"\r\n");
		Print(L"\r\n");
		if(info->HorizontalResolution==pref_width&&info->VerticalResolution==pref_height){
			mode=i;
		}
		else{
			if((info->HorizontalResolution-pref_width)*(info->VerticalResolution-pref_height)<greatest_dif){

			}
		}
	}
	if(mode>=0){
		status = uefi_call_wrapper(gop->SetMode, 2, gop, mode);

	}
	//22 is 1080p 9 is 720p//1

	if(EFI_ERROR(status)) {
		Print(L"Unable to set mode %03d", 1,"\r\n");
	} else {
		// get framebuffer
		Print(L"Framebuffer address %x size %d, width %d height %d pixelsperline %d",
			gop->Mode->FrameBufferBase,
			gop->Mode->FrameBufferSize,
			gop->Mode->Info->HorizontalResolution,
			gop->Mode->Info->VerticalResolution,
			gop->Mode->Info->PixelsPerScanLine
		);
		Print(L"\r\n");

	}

	framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	framebuffer.Height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;
	return &framebuffer;
}
static inline void PlotPixel_32bpp(int x, int y, uint32_t pixel)
{
	*((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel;
}

/**
 * This is your application's main entry point, which receives its image handle
 */
UINT32 cursor_x,cursor_y;
UINT32 console_width,console_height;
bitmap_font* console_font;

void putchar(UINT32 x, UINT32 y, CHAR8 chr){
	for (uint32_t y2 = 0; y2 < console_font->height; ++y2){
		for (uint32_t x2 = 0; x2 < console_font->width; ++x2)
		{
			if ((console_font->buffer[console_font->bytesperglyph*chr+(y2*((console_font->width+7)/8))+x2/8]>>(7-(x2%8)))&0x1){
				PlotPixel_32bpp(x+x2,y+y2,0xffffffff);
			}
		}
	}
}

void printchar(CHAR8 chr){
	if (chr=='\n'){
		cursor_x=0;
		cursor_y++;
		return;
	}
	putchar(cursor_x*console_font->width, cursor_y*console_font->height, chr);
	cursor_x++;
	if (cursor_x>=console_width){
		cursor_x=0;
		cursor_y++;
	}
}
void print(CHAR8* str){
	for (int i = 0;str[i]; ++i)
	{
		printchar(str[i]);
	}
}

void init_text_overlay(Framebuffer* buf, bitmap_font* font){
	console_font=font;
	console_width=buf->Width/font->width;
	console_height=buf->Height/font->height;
	cursor_x=0;
	cursor_y=0;
}
CHAR8 string_buf[256]={0};

CHAR8* to_string(int x){
	int index=0;
	int size=0;
	int temp=x;
	while(temp/=10){
		size++;
	}
	while(x){
		string_buf[size-index]='0'+x%10;
		x/=10;
		index++;
		
	}
	string_buf[size+1]=0x00;
	return string_buf;
}

UINTN strcmplen(CHAR8* a, CHAR8* b, UINTN length){
	for (UINTN i = 0; i < length; i++){
		if (*a != *b) return 0;
	}
	return 1;
}


EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable){
	InitializeLib(ImageHandle, SystemTable);
	Print(L"Hello, world!\n");

	EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);

	CHAR16              *FileName = L"resources\\test.txt";
	EFI_FILE_HANDLE     FileHandle;

	/* open the file */
	uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
	Print(L"Hello, world!\n");



	UINT64              ReadSize = FileSize(FileHandle);
	Print(L"Hello, world! %u \n",ReadSize);

	UINT8               *Buffer = AllocatePool(ReadSize+1);
	Buffer[ReadSize]=0x00;
	Print(L"Hello, world!\n");

	uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &ReadSize, Buffer);

	Print(L"%a\n",Buffer);

	CHAR8* conf_buf = Load_Text_File(Volume,L"resources\\config.txt");

	//CHAR8 names[16*256];
	//for (int i = 0; i < 16*256; ++i)
	//{
	//  *names=0;
	//}
	//memset(names,0,256);
	CHAR16 font_name[256]={0};
	uint32_t width=0,height=0;

	int index=0;
	int n_index=0;
	int w_index=0;

	CHAR8** names =split_string_by_char(conf_buf, '\n', &n_index);

	Print(L"HELLO\n");

	for(int i =0; i<n_index;i++){
		if(!strcmp(names[i],(CHAR8*)"#font")){
			Print(L"FOUND");
			i++;
			for (int y = 0; names[i][y]; ++y){
				font_name[y]=(CHAR16)names[i][y];
				//Print(L"%u",(uint32_t)names[i][y]);
				//printchar(names[i][y]);
			}
		}
		else if(!strcmp(names[i],(CHAR8*)"#resolution")){
			Print(L"FOUND");
			i++;
			width=atoi((char*)names[i]);
			i++;
			height=atoi((char*)names[i]);
		}
		else if(!strcmp(names[i],(CHAR8*)"#end")){
			break;
		}

	}

	Print(L"Width %u Height %u",width,height);
	//print((CHAR8*)"endnio");
	Print(font_name);

	UINT64 FileSize2=0;

	//CHAR8* font =  Load_File(Volume,L"resources\\zap-light16.psf", &FileSize2);
	//CHAR8* font =  Load_File(Volume,L"resources\\zap-light20.psf", &FileSize2);
	//CHAR8* font =  Load_File(Volume,L"resources\\ter-powerline-v28n.psf", &FileSize2);
	CHAR8* font =  Load_File(Volume,font_name, &FileSize2);

	Print(L"Font file size! %u \n",FileSize2);



	Framebuffer* newBuffer = setup_graphics(width,height);

	bitmap_font bmf=load_font(font);

	init_text_overlay(newBuffer, &bmf);


	

	//uint32_t x, y;

	for (uint32_t i = 0; i < 256; ++i){
		//x=bmf.width*(i%16);
		//y=bmf.height*(i/16);
		/*for (uint32_t y2 = 0; y2 < 28; ++y2){
			for (uint32_t x2 = 0; x2 < 14; ++x2)
			{
				if ((bmf.buffer[bmf.bytesperglyph*i+(y2*((bmf.width+7)/8))+x2/8]>>(7-(x2%8)))&0x1){
					PlotPixel_32bpp(x+x2,y+y2,0xffffffff);
				}
			}
		}*/
		//putchar(x, y, i);
		printchar(i);
	}


	Print(L"Hello, world!\n");

	CHAR8* kern =  Load_File(Volume,L"kernel.elf", &FileSize2);

	Print(L"Kernel loaded , size=%u\n",FileSize2);


	Elf64_Ehdr* header=(Elf64_Ehdr*)kern;
	Print(L"Arch: %u\n",header->e_machine);

	Print(L"entry: %u\n",header->e_entry);
	Print(L"ofset: %u\n",header->e_phoff);

	Elf64_Phdr* phdrs=(Elf64_Phdr*)(kern+header->e_phoff);

	for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header->e_phnum * header->e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header->e_phentsize)
	)
	{//load each program segment at the memory location indicated by its header in p_addr
		switch (phdr->p_type){
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				Print(L"Kernel p_paddr %u \n",phdr->p_paddr);

				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				UINTN size = phdr->p_filesz;
				for (UINTN i = 0; i < size; ++i)
				{
					((char*)segment)[i]=kern[phdr->p_offset+i];
				}
				break;
			}
		}
	}
	Print(L"Kernel e_entry %u \n",header->e_entry);
	Print(L"Kernel entry %u \n",&header->e_entry);


	//it seems since the kernes first segement is loaded at 0x0 you can just call 0. at least until the size changes
	//int (*KernelStart)()=(__attribute__((sysv_abi))int (*)())header->e_entry;
	int (*KernelStart)() = ((__attribute__((sysv_abi)) int (*)() ) header->e_entry);

	Print(L"Kernel start %u \n",&KernelStart);


	EFI_STATUS                  Status;
	EFI_MEMORY_DESCRIPTOR       *EfiMemoryMap;
	UINTN                       EfiMemoryMapSize;
	UINTN                       EfiMapKey;
	UINTN                       EfiDescriptorSize;
	UINT32                      EfiDescriptorVersion;

	//
	// Get the EFI memory map.
	//
	EfiMemoryMapSize  = 0;
	EfiMemoryMap      = NULL;
	Status = gBS->GetMemoryMap (
					&EfiMemoryMapSize,
					EfiMemoryMap,
					&EfiMapKey,
					&EfiDescriptorSize,
					&EfiDescriptorVersion
					);
	ASSERT (Status == EFI_BUFFER_TOO_SMALL);

	EfiMemoryMap = (EFI_MEMORY_DESCRIPTOR *) AllocatePool (EfiMemoryMapSize + 2 * EfiDescriptorSize);
	ASSERT (EfiMemoryMap != NULL);
	Status = gBS->GetMemoryMap (
					&EfiMemoryMapSize,
					EfiMemoryMap,
					&EfiMapKey,
					&EfiDescriptorSize,
					&EfiDescriptorVersion
					);


	Print(L"Size: %u\n",sizeof(EFI_MEMORY_DESCRIPTOR));


	Print(L"Status: %u\n",Status);


	uint64_t mem_size=0;
	uint64_t mem_size_0=0;

	printchar('8');
	for (UINTN i = 0; i < EfiMemoryMapSize/EfiDescriptorSize; ++i){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)EfiMemoryMap + (i * EfiDescriptorSize));

		Print(L"%x %u %lu  ",desc->PhysicalStart,desc->Type,desc->NumberOfPages);


		//if(desc->Type){
			mem_size+=4096*desc->NumberOfPages;
		//}
		if(desc->Type){
			mem_size_0+=4096*desc->NumberOfPages;
		}


	}
	Print(L"Mem size: %lu\n",mem_size);
	Print(L"Mem size_0: %lu\n",mem_size_0);

	EFI_CONFIGURATION_TABLE* configTable = SystemTable->ConfigurationTable;
	void* rsdp = NULL; 
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;

	for (UINTN index = 0; index < SystemTable->NumberOfTableEntries; index++){
		if (CompareGuid(&configTable[index].VendorGuid, &Acpi2TableGuid)){
			if (strcmplen((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable, 8)){
				rsdp = (void*)configTable->VendorTable;
				//break;
			}
		}
		configTable++;
	}




	CHAR8 x[]="Exiting boot services";
	print(x); 
	Print(L"Exiting boot services");

	SystemTable->BootServices->ExitBootServices(ImageHandle, EfiMapKey);

	bootinfo info;
	info.buf=newBuffer;
	info.font=&bmf;
	info.mem_map=EfiMemoryMap;
	info.map_size=EfiMemoryMapSize;
	info.map_desc_size=EfiDescriptorSize;
	info.rsdp = rsdp;
	//while (1);
	//int ret=KernelStart(newBuffer, &bmf);
	int ret=KernelStart(&info);
	Print(L"kernel returned %d\n",ret);



	cursor_x=0;
	cursor_y=console_height-1;
	print(to_string(ret));
//

	return EFI_SUCCESS;
}