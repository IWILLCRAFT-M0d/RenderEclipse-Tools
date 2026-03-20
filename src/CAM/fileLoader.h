#pragma once
#include <string>
//#include <vector>

/** @brief Abreviature meanings and format.
 * First label should always be `FS` which means `File signature`.
 * Second label will represent from what system the data comes from which can be:
 * * C    = Climax (Game specific data)
 * * RW   = RenderWare
 * * FMOD = FMOD (Shattered Memories)
 * * G    = Generic (JPGs, CSV)
 * Third label will represent what kind of data is.
 */



#define FS_C_STR_TABLE 2
#define FS_RW_RWS1     1814
#define FS_RW_RWS2     1820
#define FS_G_XAML      1180189254
#define FS_G_JPG       3774863615

namespace fileLoader {
    struct file_DataInfo {
        char*         fileData;
        unsigned long fileSize;
        std::string   fileName;
    };
    
    struct fileInfoPanel {
        wxTreeCtrl* RWSFileListTree;
    };

    extern bool fileLoaded;
    extern file_DataInfo data_info;
    extern fileInfoPanel filePanel;
    
    unsigned long char2Long(char* dataPos);
    unsigned long changeEndian(unsigned long value, bool change);
    
    void UnloadFile(void);
    void Loadfile(void);

    namespace RW {
        
        
        struct RwHeader {
            unsigned long ID;
            unsigned long chunkSize;
            unsigned long RWVersion;
        };
        
        
        
        
        
        void RWStreamFile(void);
    }
    // namespace Climax {
        
    // }
    // namespace Generic {
        
    // }
}