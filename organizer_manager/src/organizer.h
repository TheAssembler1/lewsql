#ifndef ORGANIZER_H
#define ORGANIZER_H

#include "organizer_error.h"
#include "tuple.h"

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

    virtual void push_front_record() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void pop_front_record() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void push_back_record(Tuple tuple) {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void pop_back_record() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual bool valid_record() const = 0;
};

#endif // ORGANIZER_H
