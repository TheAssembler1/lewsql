#ifndef ORGANIZER_H
#define ORGANIZER_H

#include "organizer_error.h"
#include "tuple.h"

class Organizer {
    virtual void get_next_tuple() const {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void insert_tuple(const Tuple& tuple) {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual void delete_tuple() {
        throw OrganizerError(OrganizerErrorCode::MethodNotImplemented);
    };

    virtual bool is_valid_tuple(const Tuple& tuple) const = 0;
};

#endif // ORGANIZER_H
