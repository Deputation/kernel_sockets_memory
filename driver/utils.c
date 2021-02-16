#include "common.h"

PLDR_DATA_TABLE_ENTRY get_module_by_name_x64(PEPROCESS process, wchar_t* module_name)
{
	UNICODE_STRING module_name_unicode = { 0 };
	RtlInitUnicodeString(&module_name_unicode, module_name);

	PLIST_ENTRY list = &(PsGetProcessPeb(process)->Ldr->InLoadOrderModuleList);

	for (PLIST_ENTRY entry = list->Flink; entry != list; )
	{
		PLDR_DATA_TABLE_ENTRY module = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		if (RtlCompareUnicodeString(&module->BaseDllName, &module_name_unicode, TRUE) == 0)
		{
			return module;
		}

		entry = module->InLoadOrderLinks.Flink;
	}

	return NULL;
}