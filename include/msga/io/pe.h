// Copyright (c) 2023 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#pragma once

#include "msga/io.h"
#include <list>
#include <istream>
#include <string>
#include <map>
#include <cstdint>

#define IMAGE_FILE_MACHINE_UNKNOWN 0
#define IMAGE_FILE_MACHINE_I386 0x014c
#define IMAGE_FILE_MACHINE_ARM 0x01c0
#define IMAGE_FILE_MACHINE_ARMV7 0x01c4
#define IMAGE_FILE_MACHINE_ARMNT 0x01c4
#define IMAGE_FILE_MACHINE_ARM64 0xaa64
#define IMAGE_FILE_MACHINE_AMD64 0x8664

#define IMAGE_SIZEOF_NT_OPTIONAL32_HEADER 224
#define IMAGE_SIZEOF_NT_OPTIONAL64_HEADER 240
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC 0x10b
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC 0x20b

#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_DIRECTORY_ENTRY_RESOURCE 2
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION 3
#define IMAGE_DIRECTORY_ENTRY_SECURITY 4
#define IMAGE_DIRECTORY_ENTRY_BASERELOC 5
#define IMAGE_DIRECTORY_ENTRY_DEBUG 6
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE 7
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR 8
#define IMAGE_DIRECTORY_ENTRY_TLS 9
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG 10
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT 11
#define IMAGE_DIRECTORY_ENTRY_IAT 12
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT 13
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14

#define IMAGE_SCN_SCALE_INDEX 0x00000001
#define IMAGE_SCN_TYPE_NO_PAD 0x00000008
#define IMAGE_SCN_CNT_CODE 0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA 0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA 0x00000080
#define IMAGE_SCN_LNK_OTHER 0x00000100
#define IMAGE_SCN_LNK_INFO 0x00000200
#define IMAGE_SCN_LNK_REMOVE 0x00000800
#define IMAGE_SCN_LNK_COMDAT 0x00001000
#define IMAGE_SCN_NO_DEFER_SPEC_EXC 0x00004000
#define IMAGE_SCN_GPREL 0x00008000
#define IMAGE_SCN_MEM_FARDATA 0x00008000
#define IMAGE_SCN_MEM_PURGEABLE 0x00020000
#define IMAGE_SCN_MEM_16BIT 0x00020000
#define IMAGE_SCN_MEM_LOCKED 0x00040000
#define IMAGE_SCN_MEM_PRELOAD 0x00080000

#define IMAGE_SCN_ALIGN_1BYTES 0x00100000
#define IMAGE_SCN_ALIGN_2BYTES 0x00200000
#define IMAGE_SCN_ALIGN_4BYTES 0x00300000
#define IMAGE_SCN_ALIGN_8BYTES 0x00400000
#define IMAGE_SCN_ALIGN_16BYTES 0x00500000
#define IMAGE_SCN_ALIGN_32BYTES 0x00600000
#define IMAGE_SCN_ALIGN_64BYTES 0x00700000
#define IMAGE_SCN_ALIGN_128BYTES 0x00800000
#define IMAGE_SCN_ALIGN_256BYTES 0x00900000
#define IMAGE_SCN_ALIGN_512BYTES 0x00A00000
#define IMAGE_SCN_ALIGN_1024BYTES 0x00B00000
#define IMAGE_SCN_ALIGN_2048BYTES 0x00C00000
#define IMAGE_SCN_ALIGN_4096BYTES 0x00D00000
#define IMAGE_SCN_ALIGN_8192BYTES 0x00E00000

#define IMAGE_SCN_ALIGN_MASK 0x00F00000

#define IMAGE_SCN_LNK_NRELOC_OVFL 0x01000000
#define IMAGE_SCN_MEM_DISCARDABLE 0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED 0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED 0x08000000
#define IMAGE_SCN_MEM_SHARED 0x10000000
#define IMAGE_SCN_MEM_EXECUTE 0x20000000
#define IMAGE_SCN_MEM_READ 0x40000000
#define IMAGE_SCN_MEM_WRITE 0x80000000

namespace msga
{
    namespace pe
    {
        using BYTE = uint8_t;
        using WORD = uint16_t;
        using SHORT = int16_t;
        using DWORD = uint32_t;
        using LONG = int32_t;
        using ULONGLONG = uint64_t;

        struct IMAGE_DOS_HEADER
        {
            WORD e_magic;
            WORD e_cblp;
            WORD e_cp;
            WORD e_crlc;
            WORD e_cparhdr;
            WORD e_minalloc;
            WORD e_maxalloc;
            WORD e_ss;
            WORD e_sp;
            WORD e_csum;
            WORD e_ip;
            WORD e_cs;
            WORD e_lfarlc;
            WORD e_ovno;
            WORD e_res[4];
            WORD e_oemid;
            WORD e_oeminfo;
            WORD e_res2[10];
            LONG e_lfanew;
        };

        struct IMAGE_DATA_DIRECTORY
        {
            DWORD VirtualAddress;
            DWORD Size;
        };

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

        struct IMAGE_OPTIONAL_HEADER64
        {
            WORD Magic;
            BYTE MajorLinkerVersion;
            BYTE MinorLinkerVersion;
            DWORD SizeOfCode;
            DWORD SizeOfInitializedData;
            DWORD SizeOfUninitializedData;
            DWORD AddressOfEntryPoint;
            DWORD BaseOfCode;
            ULONGLONG ImageBase;
            DWORD SectionAlignment;
            DWORD FileAlignment;
            WORD MajorOperatingSystemVersion;
            WORD MinorOperatingSystemVersion;
            WORD MajorImageVersion;
            WORD MinorImageVersion;
            WORD MajorSubsystemVersion;
            WORD MinorSubsystemVersion;
            DWORD Win32VersionValue;
            DWORD SizeOfImage;
            DWORD SizeOfHeaders;
            DWORD CheckSum;
            WORD Subsystem;
            WORD DllCharacteristics;
            ULONGLONG SizeOfStackReserve;
            ULONGLONG SizeOfStackCommit;
            ULONGLONG SizeOfHeapReserve;
            ULONGLONG SizeOfHeapCommit;
            DWORD LoaderFlags;
            DWORD NumberOfRvaAndSizes;
            IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
        };

        struct IMAGE_OPTIONAL_HEADER32
        {

            WORD Magic;
            BYTE MajorLinkerVersion;
            BYTE MinorLinkerVersion;
            DWORD SizeOfCode;
            DWORD SizeOfInitializedData;
            DWORD SizeOfUninitializedData;
            DWORD AddressOfEntryPoint;
            DWORD BaseOfCode;
            DWORD BaseOfData;
            DWORD ImageBase;
            DWORD SectionAlignment;
            DWORD FileAlignment;
            WORD MajorOperatingSystemVersion;
            WORD MinorOperatingSystemVersion;
            WORD MajorImageVersion;
            WORD MinorImageVersion;
            WORD MajorSubsystemVersion;
            WORD MinorSubsystemVersion;
            DWORD Win32VersionValue;
            DWORD SizeOfImage;
            DWORD SizeOfHeaders;
            DWORD CheckSum;
            WORD Subsystem;
            WORD DllCharacteristics;
            DWORD SizeOfStackReserve;
            DWORD SizeOfStackCommit;
            DWORD SizeOfHeapReserve;
            DWORD SizeOfHeapCommit;
            DWORD LoaderFlags;
            DWORD NumberOfRvaAndSizes;
            IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
        };

        struct IMAGE_FILE_HEADER
        {
            WORD Machine;
            WORD NumberOfSections;
            DWORD TimeDateStamp;
            DWORD PointerToSymbolTable;
            DWORD NumberOfSymbols;
            WORD SizeOfOptionalHeader;
            WORD Characteristics;
        };

#define IMAGE_SIZEOF_FILE_HEADER 20

        struct IMAGE_NT_HEADER
        {
            DWORD Signature;
            IMAGE_FILE_HEADER FileHeader;
        };

#define IMAGE_SIZEOF_SHORT_NAME 8

        struct IMAGE_RELOCATION
        {
            union
            {
                DWORD VirtualAddress;
                DWORD RelocCount;
            };
            DWORD SymbolTableIndex;
            WORD Type;
        };
#define IMAGE_SIZEOF_RELOCATION 10

        struct IMAGE_BASE_RELOCATION
        {
            DWORD VirtualAddress;
            DWORD SizeOfBlock;
        };

        struct IMAGE_BASE_RELOCATION_BLOCK
        {
            WORD offset : 12;
            WORD type : 4;
        };

#define IMAGE_SIZEOF_BASE_RELOCATION 8

#define IMAGE_REL_BASED_ABSOLUTE 0
#define IMAGE_REL_BASED_HIGH 1
#define IMAGE_REL_BASED_LOW 2
#define IMAGE_REL_BASED_HIGHLOW 3
#define IMAGE_REL_BASED_HIGHADJ 4
#define IMAGE_REL_BASED_MIPS_JMPADDR 5
#define IMAGE_REL_BASED_ARM_MOV32 5
#define IMAGE_REL_BASED_THUMB_MOV32 7
#define IMAGE_REL_BASED_MIPS_JMPADDR16 9
#define IMAGE_REL_BASED_IA64_IMM64 9
#define IMAGE_REL_BASED_DIR64 10

        struct IMAGE_SECTION_HEADER
        {
            BYTE Name[IMAGE_SIZEOF_SHORT_NAME];
            union
            {
                DWORD PhysicalAddress;
                DWORD VirtualSize;
            } Misc;
            DWORD VirtualAddress;
            DWORD SizeOfRawData;
            DWORD PointerToRawData;
            DWORD PointerToRelocations;
            DWORD PointerToLinenumbers;
            WORD NumberOfRelocations;
            WORD NumberOfLinenumbers;
            DWORD Characteristics;
        };

#define IMAGE_SIZEOF_SECTION_HEADER 40

#pragma pack(push, 2)

        struct IMAGE_SYMBOL
        {
            union
            {
                BYTE ShortName[8];
                struct
                {
                    DWORD Short;
                    DWORD Long;
                } Name;
                DWORD LongName[2];
            } N;
            DWORD Value;
            SHORT SectionNumber;
            WORD Type;
            BYTE StorageClass;
            BYTE NumberOfAuxSymbols;
        };

#pragma pack(pop)

#define IMAGE_SIZEOF_SYMBOL 18

    }

    namespace io
    {
        using namespace msga::pe;
        class pe : public io::base
        {

        public:
            using rel_pe_t=rel_sys_t<uint8_t>;
            struct section_t
            {
                IMAGE_SECTION_HEADER meta{0};
                std::vector<uint8_t> data;
                uint8_t *get() { return data.data(); }
                size_t addr() const { return meta.VirtualAddress; }
                void addr(size_t rva) { meta.VirtualAddress = rva; }
                size_t size() const { return data.size(); }
                void resize(size_t len)
                {
                    data.resize(len);
                    meta.SizeOfRawData = len;
                }

                void name(char const *name);
            };
            struct symbol_t
            {
                std::vector<IMAGE_SYMBOL> table;
                std::vector<uint8_t> data;
            };
            enum class e_option
            {
                MajorLinkerVersion,
                MinorLinkerVersion,
                SizeOfCode,
                SizeOfInitializedData,
                SizeOfUninitializedData,
                AddressOfEntryPoint,
                BaseOfCode,
                BaseOfData,
                ImageBase,
                SectionAlignment,
                FileAlignment,
                MajorOperatingSystemVersion,
                MinorOperatingSystemVersion,
                MajorImageVersion,
                MinorImageVersion,
                MajorSubsystemVersion,
                MinorSubsystemVersion,
                Win32VersionValue,
                SizeOfImage,
                SizeOfHeaders,
                CheckSum,
                Subsystem,
                DllCharacteristics,
                SizeOfStackReserve,
                SizeOfStackCommit,
                SizeOfHeapReserve,
                SizeOfHeapCommit,
                LoaderFlags,
                NumberOfRvaAndSizes,
            };

        private:
            IMAGE_DOS_HEADER m_dos_h{0};
            std::vector<uint8_t> m_dos_stub{0};
            IMAGE_NT_HEADER m_nt_h{0};
            union
            {
                IMAGE_OPTIONAL_HEADER32 m_opt32_h;
                IMAGE_OPTIONAL_HEADER64 m_opt64_h;
            };
            std::list<section_t> m_sections;
            symbol_t m_symbol;
            std::map<size_t, std::map<uint16_t, uint8_t>> m_rel_table;

            void load_relocation_table();
            void rebuild_relocation_table();
            addr_t reserved(addr_t perferred, size_t len);

        public:
            virtual io::mode arch() const override
            {
                switch (m_nt_h.FileHeader.Machine)
                {
                case IMAGE_FILE_MACHINE_AMD64:
                    return io::mode::x64;
                case IMAGE_FILE_MACHINE_I386:
                    return io::mode::x32;
                default:
                    break;
                }
                return io::mode::unknown;
            };
            virtual io::mode mode() const override { return io::mode::x32; };
            virtual void read(void *data, addr_t addr, size_t len) override;
            virtual void write(void const *data, addr_t addr, size_t len) override;
            virtual void search_rebase(rel_list_t &items, addr_t addr, size_t len) override;
            virtual void range_rebase(rel_list_t const &items, addr_t addr, size_t len) override;
            virtual void rebase(addr_t addr, rel_base_t*opt) override;
            virtual void debase(addr_t addr) override;
            virtual addr_t alloc(size_t len, addr_t preferred = 0) override;
            virtual void free(addr_t addr,size_t len) override;
            virtual bool load(std::istream &is);
            virtual bool load(std::string const&path);
            virtual bool dump(std::ostream &os);
            virtual bool dump(std::string const&path);
            virtual section_t &section(size_t rva);
            IMAGE_DATA_DIRECTORY &directory(int entry);
            void*get(addr_t va);
            template <class T>
            T &get(addr_t va)
            {
                return *reinterpret_cast<T*>(get(va));
            }
            template <class T>
            void set(size_t va, T const &&data)
            {
                auto &ref = get<T>(va);
                ref = data;
            };
            uint64_t option(e_option opt) const
            {
                switch (arch())
                {
                case io::mode::x64:
                    return option64(opt);
                case io::mode::x32:
                    return option32(opt);
                default:
                    throw new std::runtime_error("bad arch");
                }
            };
            void option(e_option opt, uint64_t value)
            {
                switch (arch())
                {
                case io::mode::x64:option64(opt, value);break;
                case io::mode::x32:option32(opt, value);break;
                default:
                    throw new std::runtime_error("bad arch");
                }
            }
            addr_t VA2RVA(addr_t va){
                return va-option(e_option::ImageBase);
            }
            addr_t RVA2VA(addr_t rva){
                return rva+option(e_option::ImageBase);
            }
            void debug()const;
        private:
            uint64_t option64(e_option opt) const;
            void option64(e_option opt, uint64_t value);
            uint64_t option32(e_option opt) const;
            void option32(e_option opt, uint64_t value);
        };
    }
}
