/*********************IMPORTANT DRAKVUF LICENSE TERMS***********************
 *                                                                         *
 * DRAKVUF (C) 2014-2019 Tamas K Lengyel.                                  *
 * Tamas K Lengyel is hereinafter referred to as the author.               *
 * This program is free software; you may redistribute and/or modify it    *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; Version 2 ("GPL"), BUT ONLY WITH ALL OF THE   *
 * CLARIFICATIONS AND EXCEPTIONS DESCRIBED HEREIN.  This guarantees your   *
 * right to use, modify, and redistribute this software under certain      *
 * conditions.  If you wish to embed DRAKVUF technology into proprietary   *
 * software, alternative licenses can be aquired from the author.          *
 *                                                                         *
 * Note that the GPL places important restrictions on "derivative works",  *
 * yet it does not provide a detailed definition of that term.  To avoid   *
 * misunderstandings, we interpret that term as broadly as copyright law   *
 * allows.  For example, we consider an application to constitute a        *
 * derivative work for the purpose of this license if it does any of the   *
 * following with any software or content covered by this license          *
 * ("Covered Software"):                                                   *
 *                                                                         *
 * o Integrates source code from Covered Software.                         *
 *                                                                         *
 * o Reads or includes copyrighted data files.                             *
 *                                                                         *
 * o Is designed specifically to execute Covered Software and parse the    *
 * results (as opposed to typical shell or execution-menu apps, which will *
 * execute anything you tell them to).                                     *
 *                                                                         *
 * o Includes Covered Software in a proprietary executable installer.  The *
 * installers produced by InstallShield are an example of this.  Including *
 * DRAKVUF with other software in compressed or archival form does not     *
 * trigger this provision, provided appropriate open source decompression  *
 * or de-archiving software is widely available for no charge.  For the    *
 * purposes of this license, an installer is considered to include Covered *
 * Software even if it actually retrieves a copy of Covered Software from  *
 * another source during runtime (such as by downloading it from the       *
 * Internet).                                                              *
 *                                                                         *
 * o Links (statically or dynamically) to a library which does any of the  *
 * above.                                                                  *
 *                                                                         *
 * o Executes a helper program, module, or script to do any of the above.  *
 *                                                                         *
 * This list is not exclusive, but is meant to clarify our interpretation  *
 * of derived works with some common examples.  Other people may interpret *
 * the plain GPL differently, so we consider this a special exception to   *
 * the GPL that we apply to Covered Software.  Works which meet any of     *
 * these conditions must conform to all of the terms of this license,      *
 * particularly including the GPL Section 3 requirements of providing      *
 * source code and allowing free redistribution of the work as a whole.    *
 *                                                                         *
 * Any redistribution of Covered Software, including any derived works,    *
 * must obey and carry forward all of the terms of this license, including *
 * obeying all GPL rules and restrictions.  For example, source code of    *
 * the whole work must be provided and free redistribution must be         *
 * allowed.  All GPL references to "this License", are to be treated as    *
 * including the terms and conditions of this license text as well.        *
 *                                                                         *
 * Because this license imposes special exceptions to the GPL, Covered     *
 * Work may not be combined (even as part of a larger work) with plain GPL *
 * software.  The terms, conditions, and exceptions of this license must   *
 * be included as well.  This license is incompatible with some other open *
 * source licenses as well.  In some cases we can relicense portions of    *
 * DRAKVUF or grant special permissions to use it in other open source     *
 * software.  Please contact tamas.k.lengyel@gmail.com with any such       *
 * requests.  Similarly, we don't incorporate incompatible open source     *
 * software into Covered Software without special permission from the      *
 * copyright holders.                                                      *
 *                                                                         *
 * If you have any questions about the licensing restrictions on using     *
 * DRAKVUF in other works, are happy to help.  As mentioned above,         *
 * alternative license can be requested from the author to integrate       *
 * DRAKVUF into proprietary applications and appliances.  Please email     *
 * tamas.k.lengyel@gmail.com for further information.                      *
 *                                                                         *
 * If you have received a written license agreement or contract for        *
 * Covered Software stating terms other than these, you may choose to use  *
 * and redistribute Covered Software under those terms instead of these.   *
 *                                                                         *
 * Source is provided to this software because we believe users have a     *
 * right to know exactly what a program is going to do before they run it. *
 * This also allows you to audit the software for security holes.          *
 *                                                                         *
 * Source code also allows you to port DRAKVUF to new platforms, fix bugs, *
 * and add new features.  You are highly encouraged to submit your changes *
 * on https://github.com/tklengyel/drakvuf, or by other methods.           *
 * By sending these changes, it is understood (unless you specify          *
 * otherwise) that you are offering unlimited, non-exclusive right to      *
 * reuse, modify, and relicense the code.  DRAKVUF will always be          *
 * available Open Source, but this is important because the inability to   *
 * relicense code has caused devastating problems for other Free Software  *
 * projects (such as KDE and NASM).                                        *
 * To specify special license conditions of your contributions, just say   *
 * so when you send them.                                                  *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the DRAKVUF   *
 * license file for more details (it's in a COPYING file included with     *
 * DRAKVUF, and also available from                                        *
 * https://github.com/tklengyel/drakvuf/COPYING)                           *
 *                                                                         *
 ***************************************************************************/

#include <config.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <glib.h>
#include <libvmi/libvmi.h>
#include <libvmi/peparse.h>

#include "private.h"
#include "linux-exports.h"
#include "linux-offsets.h"

#define PAGE_SHIFT 12
#define VM_READ		0x00000001
#define VM_WRITE	0x00000002
#define VM_EXEC		0x00000004
#define VM_SHARED	0x00000008



addr_t process_sym2va(drakvuf_t drakvuf, drakvuf_trap_info_t* info, vmi_pid_t pid, const char* lib, const char* sym)
{
    vmi_instance_t vmi = drakvuf->vmi;

    addr_t process_base = drakvuf_get_current_process(drakvuf, info);

    drakvuf_get_process_pid(drakvuf, process_base, &pid);

    addr_t mm_struct_address;
    access_context_t ctx =
    {
        .translate_mechanism = VMI_TM_PROCESS_PID,
        .addr = process_base + drakvuf->offsets[TASK_STRUCT_MMSTRUCT],
        .pid = pid
    };
    if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &mm_struct_address))
        return -1;

    addr_t mmap;
    ctx.addr = mm_struct_address + drakvuf->offsets[MM_STRUCT_MMAP];
    if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &mmap))
        return -1;

    char* libname = "";
    addr_t vm_next, nullp = 0;

    addr_t text_segment_address = 0;
    addr_t data_segment_address = 0;
    addr_t text_segment_size = 0;
    // addr_t data_segment_size;

    do
    {
        addr_t vm_start;
        ctx.addr = mmap + drakvuf->offsets[VM_AREA_STRUCT_START];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &vm_start))
            return -1;

        addr_t vm_end;
        ctx.addr = mmap + drakvuf->offsets[VM_AREA_STRUCT_END];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &vm_end))
            return -1;

        ctx.addr = mmap + drakvuf->offsets[VM_AREA_STRUCT_NEXT];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &vm_next))
            return -1;

        addr_t file_address;
        ctx.addr = mmap + drakvuf->offsets[VM_AREA_STRUCT_FILE];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &file_address))
            goto next;

        addr_t path_dentry;
        ctx.addr = file_address + drakvuf->offsets[FILE_PATH] + drakvuf->offsets[PATH_DENTRY];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &path_dentry))
            goto next;

        ctx.addr = path_dentry + drakvuf->offsets[DENTRY_D_NAME] + drakvuf->offsets[QSTR_NAME] + 16;
        libname = vmi_read_str(vmi, &ctx);
        PRINT_DEBUG("LIB NAME is: %s \n", libname);

        addr_t pgoffset;
        ctx.addr = mmap + drakvuf->offsets[VM_AREA_STRUCT_PGOFF];
        if (VMI_FAILURE == vmi_read_64(vmi, &ctx, &pgoffset))
            goto next;
        pgoffset = pgoffset << PAGE_SHIFT;

        addr_t vm_flags;
        ctx.addr = mmap + drakvuf->offsets[VM_AREA_STRUCT_FLAGS];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &vm_flags))
            goto next;

        if (strstr(libname, lib) && (vm_flags & VM_READ) && (vm_flags & VM_EXEC))
        {
            text_segment_address = vm_start;
        }

        if (strstr(libname, lib) && (vm_flags & VM_READ) && !(vm_flags & VM_WRITE)&& !(vm_flags & VM_EXEC))
        {
            data_segment_address = vm_start;
            text_segment_size = pgoffset;
        }

next:
        mmap = vm_next;

    } while (vm_next != nullp); // use strcasecmp

    // printf("text_segment_size: %lx", text_segment_size);
    // printf("data_segment_address: %lx", data_segment_address);

    ctx.translate_mechanism = VMI_TM_PROCESS_DTB;
    ctx.dtb = info->regs->cr3;

    // Parsing ELF header

    addr_t program_header_offset;
    ctx.addr = text_segment_address + drakvuf->offsets[ELF64HDR_PHOFF];
    if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &program_header_offset))
        return -1;

    short unsigned int num_of_program_headers;
    ctx.addr = text_segment_address + drakvuf->offsets[ELF64HDR_PHNUM];
    if (VMI_FAILURE == vmi_read_16(vmi, &ctx, &num_of_program_headers))
        return -1;

    short unsigned int size_of_program_headers;
    ctx.addr = text_segment_address + drakvuf->offsets[ELF64HDR_PHENTSIZE];
    if (VMI_FAILURE == vmi_read_16(vmi, &ctx, &size_of_program_headers))
        return -1;
    // Extracting DYNAMIC SEGMENT offset program headers

    addr_t dynamic_section_offset=0, dynamic_section_size=0;
    int counter =0;
    addr_t offset =0;
    while (counter < num_of_program_headers)
    {

        unsigned int ph_type;
        ctx.addr = text_segment_address + offset + program_header_offset + drakvuf->offsets[ELF64PHDR_TYPE];
        if (VMI_FAILURE == vmi_read_32(vmi, &ctx, &ph_type))
            return -1;

        addr_t ph_offset;
        ctx.addr = text_segment_address + offset + program_header_offset + drakvuf->offsets[ELF64PHDR_OFFSET];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &ph_offset))
            return -1;

        addr_t ph_memsz;
        ctx.addr = text_segment_address + offset + program_header_offset + drakvuf->offsets[ELF64PHDR_MEMSZ];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &ph_memsz))
            return -1;

        if (ph_type == 2)
        {
            dynamic_section_offset = ph_offset;
            dynamic_section_size = ph_memsz;
            break;
        }
        offset += size_of_program_headers;
        counter++;
    }

    // Exracting address of dynsym and dynstr sections from Dynamic section table entries

    addr_t dynsym_offset = 0, dynstr_offset = 0;
    // addr_t rela_section_offset =0, rela_section_size=0;
    ctx.addr = text_segment_address + dynamic_section_offset;

    if ( dynamic_section_offset > text_segment_size)
        ctx.addr = data_segment_address - text_segment_size + dynamic_section_offset;

    addr_t word, ptr;
    offset=0x0;
    while (offset < dynamic_section_size)
    {
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &word))
            return -1;
        ctx.addr += 0x8;
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &ptr))
            return -1;
        ctx.addr += 0x8;

        if (word == 0x5) //dynstr section offset
        {
            dynstr_offset = ptr;
        }

        if (word == 0x6) //dynsym section offset
        {
            dynsym_offset = ptr;
            break;
        }
        // if (word == 0x7)
        // {
        //     rela_section_offset = ptr;
        // }
        // if (word == 0x8)
        // {
        //     rela_section_size = ptr;
        //     break;
        // }
        offset+=0x10;
    }

    // printf("rela offset is %lx\n", rela_section_offset);
    // printf("rela size is %lx\n", rela_section_size);

    // Reading Relocatable files
    // offset = 0x0;
    // while (offset < 0x30)
    // {
    //     addr_t rela_addend, rela_info, rela_offset;
    //     ctx.addr = rela_section_offset + offset + drakvuf->offsets[ELF64RELA_ADDEND];
    //     if (VMI_SUCCESS == vmi_read_addr(vmi, &ctx, &rela_addend))
    //         printf("rela_addend is: %lx\n", rela_addend);

    //     ctx.addr = rela_section_offset + offset + drakvuf->offsets[ELF64RELA_INFO];
    //     if (VMI_SUCCESS == vmi_read_addr(vmi, &ctx, &rela_info))
    //         printf("rela_info is: %lx\n", rela_info);

    //     ctx.addr = rela_section_offset + offset + drakvuf->offsets[ELF64RELA_OFFSET];
    //     if (VMI_SUCCESS == vmi_read_addr(vmi, &ctx, &rela_offset))
    //         printf("rela_offset is: %lx\n\n", rela_offset);

    //     addr_t virt_addr = text_segment_address + rela_offset;

    //     ctx.addr = virt_addr;
    //     if (VMI_SUCCESS == vmi_read_addr(vmi, &ctx, &rela_offset))
    //         printf("virt addr is: %lx\n\n", rela_offset);
    //     else
    //         printf("Failed");

    //     offset+=0x18;
    // }

    // // Reading symbol name mapping in dynstr table

    // return text_segment_address;

    offset=0x0;
    addr_t dynstr_size = dynsym_offset - dynstr_offset;
    uint32_t symbol_map = 0;
    bool found = false;
    int c=0;
    while (symbol_map < dynstr_size)
    {
        char* symbol_name;
        ctx.addr = dynstr_offset + offset;
        symbol_name = vmi_read_str(vmi, &ctx);
        addr_t symbol_size = strlen(symbol_name);
        // printf("%s\n", symbol_name);
        if (strcmp(symbol_name, sym) == 0)
        {
            found = true;
            break;
        }
        offset += symbol_size+0x1;
        symbol_map += symbol_size+1;
        c++;
        if (c==3500) // to prevent from infinite loop
            break;
    }

    if (!found)
        return -1;

    // Mapping symbol name to address in dynsym table

    addr_t value;
    offset = dynsym_offset;
    while (true)
    {
        uint32_t key;
        ctx.addr =  offset + drakvuf->offsets[ELF64SYM_NAME];
        if (VMI_FAILURE == vmi_read_32(vmi, &ctx, &key))
            return -1;

        ctx.addr =  offset + drakvuf->offsets[ELF64SYM_VALUE];
        if (VMI_FAILURE == vmi_read_addr(vmi, &ctx, &value))
            return -1;

        if (key == symbol_map)
            break;
        offset += 0x18;
    }
    return text_segment_address + value;
}