#ifndef UNIQUIFIER_H
#define UNIQUIFIER_H

// Usage:
// void blurb() {
//     static bool inProgress = false;
//     if ( inProgress == true ) return;
//     Uniquifier u( &inProgress );
// // ... this code will be called only once
// }

class Uniquifier {
public:
    explicit Uniquifier( bool* guard ) {
        m_guard = guard;
        *m_guard = true;
    }
    ~Uniquifier() {
        *m_guard = false;
    }
private:
    bool *m_guard;
};

#endif
