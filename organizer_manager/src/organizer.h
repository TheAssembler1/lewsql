#ifndef ORGANIZER_H
#define ORGANIZER_H

class Organizer {
    virtual void search_record() = 0;
    virtual void insert_record() = 0;
    virtual void delete_record() = 0;
};

#endif // ORGANIZER_H
