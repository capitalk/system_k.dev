#include "venue_utils.h"

#include <string.h>

namespace capk 
{

    venue_id_t 
    micStringToVenueId(const char* mic) 
    {
       if (strncmp(mic, kFXCM_MIC_STRING, MIC_LEN)) {
           return kFXCM_VENUE_ID;
       } 
       if (strncmp(mic, kXCDE_MIC_STRING, MIC_LEN)) {
           return kXCDE_VENUE_ID;
       } 
       return kNULL_VENUE_ID;
    }

    const char* const venueIdToMICCString(const venue_id_t venue_id)
    {
        if (venue_id == kNULL_VENUE_ID) {
            return kNULL_MIC_STRING;
        }
        if (venue_id == kFXCM_VENUE_ID) {
            return kFXCM_MIC_STRING;
        }
        if (venue_id == kXCDE_VENUE_ID) {
            return kXCDE_MIC_STRING;
        }
        return kNULL_MIC_STRING;
    }

} // namespace capk
