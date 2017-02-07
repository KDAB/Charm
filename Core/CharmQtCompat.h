#ifndef CHARMQTCOMPAT_H
#define CHARMQTCOMPAT_H

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)

QT_BEGIN_NAMESPACE

// this adds const to non-const objects (like std::as_const)
template <typename T>
Q_DECL_CONSTEXPR typename std::add_const<T>::type &qAsConst(T &t) Q_DECL_NOTHROW { return t; }

// prevent rvalue arguments:
template <typename T>
void qAsConst(const T &&) Q_DECL_EQ_DELETE;

QT_END_NAMESPACE

#endif

#endif // CHARMQTCOMPAT_H
