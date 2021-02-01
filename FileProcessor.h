#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include "Exception.h"
#include "IoPool.h"
#include "Util.h"
#include "winUtil.h"

inline const int ConcurrentFiles = 1000; // Copy 1000 files simultaneously

template<class Processor> class FileProcessor : public utils::ISearchExaminer3
{
    const io::IoPool &        m_ioPool;
    const std::wstring &      m_srcPath;
    const std::wstring &      m_dstPath;
    uint64_t                  m_totalFilesProcessed;
    std::vector<std::wstring>                           m_files;
    std::function<void(std::wstring &, std::wstring &)> m_preprocessor;

public:
    FileProcessor(const io::IoPool &ioPool, const std::wstring &srcPath, const std::wstring &dstPath)
        : m_ioPool(ioPool)
        , m_srcPath(srcPath)
        , m_dstPath(dstPath)
        , m_totalFilesProcessed(0)
    {
    }
    uint64_t TotalProcessed() const
    {
        return m_totalFilesProcessed;
    }
    void SetPreprocessor(std::function<void(std::wstring &, std::wstring &)> f)
    {
        m_preprocessor = f;
    }

private:
    void ProcessFiles()
    {
        const std::size_t fileCount = m_files.size();

        HANDLE completeEvent = ::CreateEventA(nullptr, TRUE, FALSE, nullptr);
        ex::CheckZero(completeEvent, "ProcessFiles. Create event");

        util::HandleGuard eventGuard(completeEvent);

        std::atomic_int64_t filesProcessed {0};
        auto                onFileComplete = [&filesProcessed, completeEvent, fileCount]() {
            if (++filesProcessed == (int64_t)fileCount)
                ::SetEvent(completeEvent);
        };

        for (auto &srcFile : m_files)
        {
            try
            {
                std::wstring dstFile = srcFile;
                m_preprocessor(srcFile, dstFile);

                std::shared_ptr<Processor> processor = std::make_shared<Processor>(m_ioPool, srcFile, dstFile, onFileComplete);

                processor->Start();
            }
            catch (const std::exception &ex)
            {
                std::cerr << ex.what() << "\n";
            }
        }

        ::WaitForSingleObject(completeEvent, INFINITE);
        m_totalFilesProcessed += filesProcessed;
    }
    // cmn::ISearchExaminer3
    void OnCannotStart(const std::wstring &, DWORD dwError)
    {
        std::cerr << "Can't start search, error = " << dwError << "\n";
    }
    void OnFileFound(const std::wstring &path, WIN32_FIND_DATAW *findData)
    {
        if (path.empty())
            return;

        std::wstring filePath = path;
        if (filePath.back() != L'\\' && filePath.back() != L'/')
        {
            filePath.push_back(L'\\');
        }

        filePath.append(findData->cFileName);

        m_files.push_back(std::move(filePath));
        if (m_files.size() != ConcurrentFiles)
            return;

        ProcessFiles();
        m_files.clear();
    }
    void OnDirectoryFound(const std::wstring &, WIN32_FIND_DATAW *) {}
    void OnDoneDirectory(const std::wstring &path)
    {
        if (path.empty() || path != m_srcPath)
            return;

        ProcessFiles();
        m_files.clear();
    }
    bool CanProcessDirectory(const std::wstring &, WIN32_FIND_DATAW *)
    {
        return true;
    }
};

#endif // FILEPROCESSOR_H
