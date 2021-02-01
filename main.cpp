#include "StdAfx.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QTextStream>
#include <QThread>

#include "IoPool.h"
#include "ProcessFiles.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("cbim.com.cn");
    QCoreApplication::setApplicationName("WinCopy");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("WinCopy");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption algoOpt(
        QStringList() << "algorithm",
        QCoreApplication::translate("main",
                                    "hash digest algorithm, cadidates: md4, md5, sha1, sha224, sha256, sha384, sha512, sha3_224, "
                                    "sha3_256, sha3_384, sha3_512, keccak_224, keccak_256, keccak_384, keccak_512"),
        QCoreApplication::translate("main", "algorithm"),
        "md5");
    parser.addOption(algoOpt);
    QCommandLineOption digestOpt(QStringList() << "c"
                                               << "digest",
                                 QCoreApplication::translate("main", "file that stores files' hash digest"),
                                 QCoreApplication::translate("main", "file"),
                                 QString("%1\\digest.txt").arg(QDir::currentPath()));
    parser.addOption(digestOpt);
    QCommandLineOption srcOpt(QStringList() << "s"
                                            << "source",
                              QCoreApplication::translate("main", "source directory path"),
                              QCoreApplication::translate("main", "directory"));
    parser.addOption(srcOpt);
    QCommandLineOption dstOpt(QStringList() << "d"
                                            << "destine",
                              QCoreApplication::translate("main", "destine directory path"),
                              QCoreApplication::translate("main", "directory"));
    parser.addOption(dstOpt);
    QCommandLineOption actionOpt(QStringList() << "a"
                                               << "action",
                                 QCoreApplication::translate("main", "action to take, cadidates: copy, digest, verify"),
                                 QCoreApplication::translate("main", "action"),
                                 "copy");
    parser.addOption(actionOpt);
    parser.process(a);

    QString     action = parser.value(actionOpt);
    QTextStream ts(stderr);
    ts.setCodec("UTF-8");

    const wchar_t slash = L'\\';
    if (action.toLower() == "copy")
    {
        try
        {
            QString srcDir = parser.value(srcOpt);
            if (srcDir.isEmpty())
            {
                ts << "Missing source directory";
                parser.showHelp(1);
            }
            std::wstring src = QDir::toNativeSeparators(srcDir).toStdWString();
            if (src.back() != slash)
                src.push_back(slash);

            QString dstDir = parser.value(dstOpt);
            if (dstDir.isEmpty())
            {
                ts << "Missing destine directory";
                parser.showHelp(1);
            }
            std::wstring dst = QDir::toNativeSeparators(dstDir).toStdWString();
            if (dst.back() != slash)
                dst.push_back(slash);

            io::IoPool ioPool(QThread::idealThreadCount() > 4 ? 4 : QThread::idealThreadCount());
            ProcessFiles::CopyFiles(ioPool, src, dst);
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Error: " << ex.what() << std::endl;
            return 1;
        }
        return 0;
    }

    if (action.toLower() == "digest")
    {
        QString srcDir = parser.value(srcOpt);
        if (srcDir.isEmpty())
        {
            ts << "Missing source directory";
            parser.showHelp(1);
        }
        std::wstring src = QDir::toNativeSeparators(srcDir).toStdWString();
        if (src.back() != slash)
            src.push_back(slash);

        QString digestFile = parser.value(digestOpt);
        if (digestFile.isEmpty())
        {
            ts << "Missing digest file path";
            parser.showHelp(1);
        }

        io::IoPool ioPool(QThread::idealThreadCount() > 4 ? 4 : QThread::idealThreadCount());
        ProcessFiles::DigestFiles(ioPool, src, digestFile.toStdWString());
        return 0;
    }

    if (action.toLower() == "verify")
    {
        QString dstDir = parser.value(dstOpt);
        if (dstDir.isEmpty())
        {
            ts << "Missing destine directory";
            parser.showHelp(1);
        }
        std::wstring dst = QDir::toNativeSeparators(dstDir).toStdWString();
        if (dst.back() != slash)
            dst.push_back(slash);

        QString digestFile = parser.value(digestOpt);
        if (digestFile.isEmpty())
        {
            ts << "Missing digest file path";
            parser.showHelp(1);
        }

        io::IoPool ioPool(QThread::idealThreadCount() > 4 ? 4 : QThread::idealThreadCount());
        ProcessFiles::VerifyFiles(ioPool, dst, digestFile.toStdWString());
        return 0;
    }

    ts << "Unsupported action.";
    return -1;
}
