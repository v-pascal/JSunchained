#include "Storage.h"

#if defined(TARGET_OS_ANDROID) || defined(TARGET_OS_WINDOWS)
#include <Unchained/Log/Log.h>
#else
#include "Log.h"
#endif
#include <boost/filesystem.hpp>
#include <fstream>

#define DEFAULT_JPEG        "default.jpg"


//////
Storage::Storage(char** response) : Reply(response), mPicIdx(UNCHAINED_NO_DATA) {

    LOGV(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(" - r:%p"), __PRETTY_FUNCTION__, __LINE__, response);
}
Storage::~Storage() {

    LOGV(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    for (std::vector<std::string*>::iterator iter = mPictures.begin(); iter != mPictures.end(); ++iter)
        delete (*iter);
    mPictures.clear();
}

std::string Storage::getFolder(unsigned char type) {

    LOGV(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(" - t:%d"), __PRETTY_FUNCTION__, __LINE__, type);
    std::string folder; // Error (empty)

#ifdef TARGET_OS_ANDROID
    LOGI(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(" - (j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__, g_jVM, g_jResClass,
            g_jResObj);
    assert((type == FOLDER_TYPE_PICTURES) || (type == FOLDER_TYPE_MOVIES) || (type == FOLDER_TYPE_APPLICATION));
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_STORAGE, __PRETTY_FUNCTION__, __LINE__);
    if (!env)
        return folder;

    jmethodID mthd = env->GetMethodID(g_jResClass, "getFolder", "(S)Ljava/lang/String;");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'getFolder' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return folder;
    }
    jstring strFolder = static_cast<jstring>(env->CallObjectMethod(g_jResObj, mthd, static_cast<short>(type)));
    if (strFolder) {

        const char* lpFolder = env->GetStringUTFChars(strFolder, 0);
        folder.assign(lpFolder);
        env->ReleaseStringUTFChars(strFolder, lpFolder);

        // Check avoid #JNI1 issue case
        if (folder == " ") folder.clear(); // null
    }
#ifdef DEBUG
    else { // #JNI1 issue found

        LOGF(LOG_FORMAT(" - #JNI1 issue found"), __PRETTY_FUNCTION__, __LINE__);
        //assert(NULL); // Let's #JNI1 crash
        folder.clear();
    }
#endif

#elif defined(TARGET_OS_WINDOWS)


#else
    NSSearchPathDirectory dirType;
    switch (type) {

        case FOLDER_TYPE_PICTURES: dirType = NSPicturesDirectory; break;
        case FOLDER_TYPE_MOVIES: dirType = NSMoviesDirectory; break;
        case FOLDER_TYPE_DOCUMENTS: dirType = NSDocumentDirectory; break;
        default: {

            LOGF(LOG_FORMAT(" - Unknown folder type"), __PRETTY_FUNCTION__, __LINE__);
            assert(NULL);
            return folder;
        }
    }
    NSArray* pathList = NSSearchPathForDirectoriesInDomains(dirType, NSUserDomainMask, YES);
    if ([pathList count] == 0) {
        pathList = NSSearchPathForDirectoriesInDomains(dirType, NSLocalDomainMask, YES);
        if ([pathList count] == 0)
            pathList = NSSearchPathForDirectoriesInDomains(dirType, NSAllDomainsMask, YES);
    }
    if ([pathList count] > 0)
        folder.assign([[NSFileManager defaultManager] fileSystemRepresentationWithPath:[pathList objectAtIndex:0]]);
#ifdef DEBUG
    else {

        LOGE(LOG_FORMAT(" - Failed to get '%d' folder"), __PRETTY_FUNCTION__, __LINE__, type);
        assert(NULL);
    }
#endif
#endif
    return folder;
}

void Storage::init() {

    LOGV(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#ifdef TARGET_OS_ANDROID
    using namespace boost::filesystem;
    path picDir(getFolder(FOLDER_TYPE_PICTURES));
    directory_iterator end;
    for (directory_iterator dir(picDir) ; dir != end; ++dir)
        if ((is_regular_file(dir->status())) && ((!dir->path().extension().compare(".jpg")) ||
                (!dir->path().extension().compare(".JPG"))))
            mPictures.push_back(new std::string(dir->path().generic_string()));
#elif defined(TARGET_OS_WINDOWS)

#else
    NSLog(@"WARNING: Storage not managed yet!");
    //assert(NULL);
#endif
}

bool Storage::defaultReply() {

    LOGV(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#ifdef TARGET_OS_ANDROID
    LOGI(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(" - (j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__, g_jVM, g_jResClass,
            g_jResObj);
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_STORAGE, __PRETTY_FUNCTION__, __LINE__);
    if (!env) {

        mLength = 0;
        return false;
    }
    jmethodID mthd = env->GetMethodID(g_jResClass, "openAsset", "(Ljava/lang/String;)[B");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'openAsset' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        mLength = 0;
        return false;
    }
    jstring jFile = env->NewStringUTF(DEFAULT_JPEG);
    jbyteArray jBytes = static_cast<jbyteArray>(env->CallObjectMethod(g_jResObj, mthd, jFile));
    //assert(jBytes); // To be tested!
    env->NewLocalRef(jBytes);
    env->DeleteLocalRef(jFile);

    mLength = env->GetArrayLength(jBytes);
    jbyte* bytes = static_cast<jbyte*>(env->GetByteArrayElements(jBytes, 0));
    checkIncBuffer();
    memcpy(*mResponse, reinterpret_cast<char*>(bytes), mLength);
    env->ReleaseByteArrayElements(jBytes, bytes, 0 );
    env->DeleteLocalRef(jBytes);

#else



    //NotImplementedYet();



#endif
    return true;
}
bool Storage::reply(const void* data) {

    LOGV(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(" - d:%p (i:%d)"), __PRETTY_FUNCTION__, __LINE__, data, mPicIdx);
    ++mPicIdx;
    if (mPicIdx >= mPictures.size()) {

        mPicIdx = 0;
        if (mPictures.empty())
            return defaultReply();
    }
    LOGI(UNCHAINED_LOG_STORAGE, 0, LOG_FORMAT(" - %s"), __PRETTY_FUNCTION__, __LINE__, mPictures[mPicIdx]->c_str());
    std::ifstream ifs(mPictures[mPicIdx]->c_str(), std::ifstream::binary);
    if (!ifs.is_open()) {

        LOGE(LOG_FORMAT(" - ERROR: Failed to open image (%s)"), __PRETTY_FUNCTION__, __LINE__, mPictures[mPicIdx]->c_str());
        return defaultReply();
    }
    std::filebuf* pbuf = ifs.rdbuf();
    mLength = static_cast<int>(pbuf->pubseekoff(0, ifs.end, ifs.in));
    if (mLength < 1) {

        ifs.close();
        LOGE(LOG_FORMAT(" - ERROR: Wrong image size (%s)"), __PRETTY_FUNCTION__, __LINE__, mPictures[mPicIdx]->c_str());
        return defaultReply();
    }
    checkIncBuffer();
    pbuf->pubseekpos(0, ifs.in);
    pbuf->sgetn(*mResponse, mLength);
    ifs.close();

    return true;
}
