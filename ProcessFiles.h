#pragma once

namespace io
{
    class IoPool;
} // namespace io

namespace ProcessFiles
{
    void CopyFiles(const io::IoPool& ioPool, const std::wstring& src, const std::wstring& dst);
    void DigestFiles(const io::IoPool &ioPool, const std::wstring &src, const std::wstring &file);
    void VerifyFiles(const io::IoPool &ioPool, const std::wstring &dst, const std::wstring &file);
} // namespace ProcessFiles
