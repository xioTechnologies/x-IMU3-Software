#pragma once

#include "../../C/Ximu3.h"
#include "EventArgs.h"
#include "FileConverterProgress.h"
#include "Helpers.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Ximu3
{
    public ref class FileConverter
    {
    public:
        FileConverter(String^ destination, String^ name, array<String^>^ files, EventHandler<FileConverterEventArgs^>^ fileConverterProgressEvent) : fileConverterProgressEvent{ fileConverterProgressEvent }
        {
            const auto charPtrVector = Helpers::ToCharPtrVector(files);
            fileConverter = ximu3::XIMU3_file_converter_new(Helpers::ToCharPtr(destination), Helpers::ToCharPtr(name), charPtrVector.data(), (uint32_t)charPtrVector.size(), static_cast<ximu3::XIMU3_CallbackFileConverterProgress>(Marshal::GetFunctionPointerForDelegate(fileConverterProgressDelegate).ToPointer()), GCHandle::ToIntPtr(thisHandle).ToPointer());
        }

        ~FileConverter() {
            ximu3::XIMU3_file_converter_free(fileConverter);
        }

        static FileConverterProgress^ Convert(String^ destination, String^ name, array<String^>^ files)
        {
            const auto charPtrVector = Helpers::ToCharPtrVector(files);
            return gcnew FileConverterProgress(ximu3::XIMU3_file_converter_convert(Helpers::ToCharPtr(destination), Helpers::ToCharPtr(name), charPtrVector.data(), (uint32_t)charPtrVector.size()));
        }

    private:
        ximu3::XIMU3_FileConverter* fileConverter;

        GCHandle thisHandle = GCHandle::Alloc(this, GCHandleType::Weak);

        EventHandler<FileConverterEventArgs^>^ fileConverterProgressEvent;

        delegate void FileConverterProgressDelegate(ximu3::XIMU3_FileConverterProgress data, void* context);

        static void FileConverterProgressCallback(ximu3::XIMU3_FileConverterProgress data, void* context)
        {
            auto sender = GCHandle::FromIntPtr(IntPtr(context)).Target;
            static_cast<FileConverter^>(sender)->fileConverterProgressEvent(sender, gcnew FileConverterEventArgs(gcnew FileConverterProgress(data)));
        }

        const FileConverterProgressDelegate^ fileConverterProgressDelegate = gcnew FileConverterProgressDelegate(FileConverterProgressCallback);
    };
}
