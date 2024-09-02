#ifndef ORGANIZER_H
#define ORGANIZER_H

#include "organizer_error.h"

class Organizer {
    virtual void search_record() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void insert_record() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void delete_record() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void append_record() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void prepend_record() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };
};

#endif // ORGANIZER_H
