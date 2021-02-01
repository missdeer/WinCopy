#include "StdAfx.h"

#include "ProcessFiles.h"

#include "CopyFile.h"
#include "DigestFile.h"
#include "Exception.h"
#include "FileProcessor.h"
#include "OvlFile.h"
#include "Util.h"
#include "VerifyFile.h"
#include "winUtil.h"

namespace ProcessFiles
{
    void CopyFiles(const io::IoPool &ioPool, const std::wstring &src, const std::wstring &dst)
    {
        FileProcessor<copy::CopyFile> copier(ioPool, src, dst);
        utils::CSearcher searcher(&copier);
        searcher.StartSearch(L"*", src, 0);

        std::cout << copier.TotalProcessed() << " files processed\n";
    }

    void DigestFiles(const io::IoPool &ioPool, const std::wstring &src, const std::wstring &file)
    {
        FileProcessor<digest::DigestFile> digester(ioPool, src, file);
        utils::CSearcher                  searcher(&digester);
        searcher.StartSearch(L"*", src, 0);

        std::cout << digester.TotalProcessed() << " files processed\n";
    }

    void VerifyFiles(const io::IoPool &ioPool, const std::wstring &dst, const std::wstring &file)
    {
        FileProcessor<verify::VerifyFile> verifier(ioPool, dst, file);
        utils::CSearcher                  searcher(&verifier);
        searcher.StartSearch(L"*", dst, 0);

        std::cout << verifier.TotalProcessed() << " files processed\n";
    }
} // namespace ProcessFiles
