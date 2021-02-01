#ifndef COPYFILE_H
#define COPYFILE_H

#include "Exception.h"
#include "OvlFile.h"
#include "Util.h"
#include "winUtil.h"

namespace copy
{
    static const size_t BufferSize = 0x4000; // Use 16K read/write buffers

    class CopyFile : public std::enable_shared_from_this<CopyFile>
    {
    public:
        using CompleteHandler_t = std::function<void()>;

        CopyFile(const io::IoPool &ioPool, const std::wstring &srcPath, const std::wstring &dstPath, CompleteHandler_t onComplete)
            : m_srcFile(ioPool, srcPath.c_str(), OPEN_EXISTING, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE)
            , m_srcSize(m_srcFile.Size())
            , m_dstFile(ioPool, dstPath.c_str(), CREATE_ALWAYS)
            , m_buf(BufferSize)
            , m_onComplete(onComplete)
            , m_copied(0)
        {
        }
        void Start()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            StartRead();
        }

    private:
        void StartRead()
        {
            io::CompleteHandler_t completeHandler = std::bind(std::mem_fn(&CopyFile::OnReadComplete), shared_from_this(), std::placeholders::_1);

            m_srcFile.StartRead(m_copied, &m_buf[0], static_cast<DWORD>(m_buf.size()), completeHandler);
        }
        void OnReadComplete(DWORD transferred)
        {
            try
            {
                if (!transferred)
                {
                    m_onComplete();
                    return;
                }

                std::lock_guard<std::mutex> lock(m_mutex);

                io::CompleteHandler_t completeHandler = std::bind(std::mem_fn(&CopyFile::OnWriteComplete), shared_from_this(), std::placeholders::_1);

                m_dstFile.StartWrite(m_copied, &m_buf[0], transferred, completeHandler);
            }
            catch (const std::exception &)
            {
            }
        }
        void OnWriteComplete(DWORD transferred)
        {
            try
            {
                if (!transferred)
                {
                    m_onComplete();
                    return;
                }

                std::lock_guard<std::mutex> lock(m_mutex);
                m_copied += transferred;

                if (m_copied == m_srcSize)
                {
                    m_onComplete();
                    return;
                }

                StartRead();
            }
            catch (const std::exception &)
            {
            }
        }

    private:
        io::OvlFile        m_srcFile;
        const uint64_t     m_srcSize;
        io::OvlFile        m_dstFile;
        std::vector<char>  m_buf;
        CompleteHandler_t  m_onComplete;
        mutable std::mutex m_mutex;
        uint64_t           m_copied;
    };

} // namespace copy

#endif // COPYFILE_H
