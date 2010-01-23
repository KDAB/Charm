#include "MacCarbonApplication.h"

#include <Carbon/Carbon.h>

#include <QKeyEvent>
#include <QShortcut>
#include <QTextCodec>

/* key maps */
struct qt_mac_enum_mapper
{
    int mac_code;
    int qt_code;
#define QT_MAC_MAP_ENUM(x) x, #x
    const char *desc;
};

//keyboard keys (non-modifiers)
static qt_mac_enum_mapper qt_mac_keyboard_symbols[] = {
    { kHomeCharCode, QT_MAC_MAP_ENUM(Qt::Key_Home) },
    { kEnterCharCode, QT_MAC_MAP_ENUM(Qt::Key_Enter) },
    { kEndCharCode, QT_MAC_MAP_ENUM(Qt::Key_End) },
    { kBackspaceCharCode, QT_MAC_MAP_ENUM(Qt::Key_Backspace) },
    { kTabCharCode, QT_MAC_MAP_ENUM(Qt::Key_Tab) },
    { kPageUpCharCode, QT_MAC_MAP_ENUM(Qt::Key_PageUp) },
    { kPageDownCharCode, QT_MAC_MAP_ENUM(Qt::Key_PageDown) },
    { kReturnCharCode, QT_MAC_MAP_ENUM(Qt::Key_Return) },
    { kEscapeCharCode, QT_MAC_MAP_ENUM(Qt::Key_Escape) },
    { kLeftArrowCharCode, QT_MAC_MAP_ENUM(Qt::Key_Left) },
    { kRightArrowCharCode, QT_MAC_MAP_ENUM(Qt::Key_Right) },
    { kUpArrowCharCode, QT_MAC_MAP_ENUM(Qt::Key_Up) },
    { kDownArrowCharCode, QT_MAC_MAP_ENUM(Qt::Key_Down) },
    { kHelpCharCode, QT_MAC_MAP_ENUM(Qt::Key_Help) },
    { kDeleteCharCode, QT_MAC_MAP_ENUM(Qt::Key_Delete) },
//ascii maps, for debug
    { ':', QT_MAC_MAP_ENUM(Qt::Key_Colon) },
    { ';', QT_MAC_MAP_ENUM(Qt::Key_Semicolon) },
    { '<', QT_MAC_MAP_ENUM(Qt::Key_Less) },
    { '=', QT_MAC_MAP_ENUM(Qt::Key_Equal) },
    { '>', QT_MAC_MAP_ENUM(Qt::Key_Greater) },
    { '?', QT_MAC_MAP_ENUM(Qt::Key_Question) },
    { '@', QT_MAC_MAP_ENUM(Qt::Key_At) },
    { ' ', QT_MAC_MAP_ENUM(Qt::Key_Space) },
    { '!', QT_MAC_MAP_ENUM(Qt::Key_Exclam) },
    { '"', QT_MAC_MAP_ENUM(Qt::Key_QuoteDbl) },
    { '#', QT_MAC_MAP_ENUM(Qt::Key_NumberSign) },
    { '$', QT_MAC_MAP_ENUM(Qt::Key_Dollar) },
    { '%', QT_MAC_MAP_ENUM(Qt::Key_Percent) },
    { '&', QT_MAC_MAP_ENUM(Qt::Key_Ampersand) },
    { '\'', QT_MAC_MAP_ENUM(Qt::Key_Apostrophe) },
    { '(', QT_MAC_MAP_ENUM(Qt::Key_ParenLeft) },
    { ')', QT_MAC_MAP_ENUM(Qt::Key_ParenRight) },
    { '*', QT_MAC_MAP_ENUM(Qt::Key_Asterisk) },
    { '+', QT_MAC_MAP_ENUM(Qt::Key_Plus) },
    { ',', QT_MAC_MAP_ENUM(Qt::Key_Comma) },
    { '-', QT_MAC_MAP_ENUM(Qt::Key_Minus) },
    { '.', QT_MAC_MAP_ENUM(Qt::Key_Period) },
    { '/', QT_MAC_MAP_ENUM(Qt::Key_Slash) },
    { '[', QT_MAC_MAP_ENUM(Qt::Key_BracketLeft) },
    { ']', QT_MAC_MAP_ENUM(Qt::Key_BracketRight) },
    { '\\', QT_MAC_MAP_ENUM(Qt::Key_Backslash) },
    { '_', QT_MAC_MAP_ENUM(Qt::Key_Underscore) },
    { '`', QT_MAC_MAP_ENUM(Qt::Key_QuoteLeft) },
    { '{', QT_MAC_MAP_ENUM(Qt::Key_BraceLeft) },
    { '}', QT_MAC_MAP_ENUM(Qt::Key_BraceRight) },
    { '|', QT_MAC_MAP_ENUM(Qt::Key_Bar) },
    { '~', QT_MAC_MAP_ENUM(Qt::Key_AsciiTilde) },
    { '^', QT_MAC_MAP_ENUM(Qt::Key_AsciiCircum) },
    {   0, QT_MAC_MAP_ENUM(0) }
};

static qt_mac_enum_mapper qt_mac_keyvkey_symbols[] = { //real scan codes
    { 122, QT_MAC_MAP_ENUM(Qt::Key_F1) },
    { 120, QT_MAC_MAP_ENUM(Qt::Key_F2) },
    { 99,  QT_MAC_MAP_ENUM(Qt::Key_F3) },
    { 118, QT_MAC_MAP_ENUM(Qt::Key_F4) },
    { 96,  QT_MAC_MAP_ENUM(Qt::Key_F5) },
    { 97,  QT_MAC_MAP_ENUM(Qt::Key_F6) },
    { 98,  QT_MAC_MAP_ENUM(Qt::Key_F7) },
    { 100, QT_MAC_MAP_ENUM(Qt::Key_F8) },
    { 101, QT_MAC_MAP_ENUM(Qt::Key_F9) },
    { 109, QT_MAC_MAP_ENUM(Qt::Key_F10) },
    { 103, QT_MAC_MAP_ENUM(Qt::Key_F11) },
    { 111, QT_MAC_MAP_ENUM(Qt::Key_F12) },
    { 105, QT_MAC_MAP_ENUM(Qt::Key_F13) },
    { 107, QT_MAC_MAP_ENUM(Qt::Key_F14) },
    { 113, QT_MAC_MAP_ENUM(Qt::Key_F15) },
    { 106, QT_MAC_MAP_ENUM(Qt::Key_F16) },
    {   0, QT_MAC_MAP_ENUM(0) }
};

static int qt_mac_get_key(int modif, const QChar &key, int virtualKey)
{
    if(key == kClearCharCode && virtualKey == 0x47)
        return Qt::Key_Clear;

    if(key.isDigit()) {
        return key.digitValue() + Qt::Key_0;
    }

    if(key.isLetter()) {
        return (key.toUpper().unicode() - 'A') + Qt::Key_A;
    }
    if(key.isSymbol()) {
        return key.unicode();
    }

    for(int i = 0; qt_mac_keyboard_symbols[i].qt_code; i++) {
        if(qt_mac_keyboard_symbols[i].mac_code == key) {
            /* To work like Qt for X11 we issue Backtab when Shift + Tab are pressed */
            if(qt_mac_keyboard_symbols[i].qt_code == Qt::Key_Tab && (modif & Qt::ShiftModifier)) {
                return Qt::Key_Backtab;
            }

            return qt_mac_keyboard_symbols[i].qt_code;
        }
    }

    //last ditch try to match the scan code
    for(int i = 0; qt_mac_keyvkey_symbols[i].qt_code; i++) {
        if(qt_mac_keyvkey_symbols[i].mac_code == virtualKey) {
            return qt_mac_keyvkey_symbols[i].qt_code;
        }
    }

    //oh well
    return Qt::Key_unknown;
}

//modifiers
static qt_mac_enum_mapper qt_mac_modifier_symbols[] = {
    { shiftKey, QT_MAC_MAP_ENUM(Qt::ShiftModifier) },
    { rightShiftKey, QT_MAC_MAP_ENUM(Qt::ShiftModifier) },
    { controlKey, QT_MAC_MAP_ENUM(Qt::MetaModifier) },
    { rightControlKey, QT_MAC_MAP_ENUM(Qt::MetaModifier) },
    { cmdKey, QT_MAC_MAP_ENUM(Qt::ControlModifier) },
    { optionKey, QT_MAC_MAP_ENUM(Qt::AltModifier) },
    { rightOptionKey, QT_MAC_MAP_ENUM(Qt::AltModifier) },
    { kEventKeyModifierNumLockMask, QT_MAC_MAP_ENUM(Qt::KeypadModifier) },
    { 0, QT_MAC_MAP_ENUM(0) }
};
static Qt::KeyboardModifiers qt_mac_get_modifiers( int keys )
{
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
    qDebug("Qt: internal: **Mapping modifiers: %d (0x%04x)", keys, keys);
#endif
    Qt::KeyboardModifiers ret = Qt::NoModifier;
    for(int i = 0; qt_mac_modifier_symbols[i].qt_code; i++) {
        if(keys & qt_mac_modifier_symbols[i].mac_code) {
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
            qDebug("Qt: internal: got modifier: %s", qt_mac_modifier_symbols[i].desc);
#endif
            ret |= Qt::KeyboardModifier(qt_mac_modifier_symbols[i].qt_code);
        }
    }
    return ret;
}

static bool qt_mac_eat_unicode_key = false;

static bool translateKeyEventInternal(EventHandlerCallRef er, EventRef keyEvent, int *qtKey,
                                      QChar *outChar, Qt::KeyboardModifiers *outModifiers, bool *outHandled)
{
    const UInt32 ekind = GetEventKind(keyEvent);
    {
        UInt32 mac_modifiers = 0;
        GetEventParameter(keyEvent, kEventParamKeyModifiers, typeUInt32, 0,
                          sizeof(mac_modifiers), 0, &mac_modifiers);
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
        qDebug("************ Mapping modifiers and key ***********");
#endif
        *outModifiers = qt_mac_get_modifiers(mac_modifiers);
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
        qDebug("------------ Mapping modifiers and key -----------");
#endif
    }

    //get keycode
    UInt32 keyCode = 0;
    GetEventParameter(keyEvent, kEventParamKeyCode, typeUInt32, 0, sizeof(keyCode), 0, &keyCode);

    //get mac mapping
    static UInt32 tmp_unused_state = 0L;
    const UCKeyboardLayout *uchrData = 0;
#ifdef Q_OS_MAC32
    Q_UNUSED(er);
    Q_UNUSED(outHandled);
    KeyboardLayoutRef keyLayoutRef = 0;
    KLGetCurrentKeyboardLayout(&keyLayoutRef);
    OSStatus err;
    if(keyLayoutRef != 0) {
        err = KLGetKeyboardLayoutProperty(keyLayoutRef, kKLuchrData,
                                  (reinterpret_cast<const void **>(&uchrData)));
        if(err != noErr) {
            qWarning("Qt::internal::unable to get keyboardlayout %ld %s:%d",
                     long(err), __FILE__, __LINE__);
        }
    }
#else
    QCFType<TISInputSourceRef> inputSource = TISCopyCurrentKeyboardInputSource();
    if (!inputSource) {
        qWarning("Qt: QKeyMapper::translateKeyEvent: could not get current keyboard input source (%s:%d)", __FILE__, __LINE__);
        return false;
    }
    CFDataRef data = static_cast<CFDataRef>(TISGetInputSourceProperty(inputSource, kTISPropertyUnicodeKeyLayoutData));
    if (!data) {
        qWarning("Qt: QKeyMapper::translateKeyEvent: could not get current keyboard unicode layout data (%s:%d)", __FILE__, __LINE__);
        return false;
    }
    uchrData = reinterpret_cast<const UCKeyboardLayout *>(CFDataGetBytePtr(data));
#endif
    *qtKey = Qt::Key_unknown;
    if(uchrData) {
        // The easy stuff; use the unicode stuff!
        UniChar string[4];
        UniCharCount actualLength;
        UInt32 currentModifiers = GetCurrentEventKeyModifiers();
        UInt32 currentModifiersWOAltOrControl = currentModifiers & ~(controlKey | optionKey);
        int keyAction;
        switch (ekind) {
        default:
        case kEventRawKeyDown:
            keyAction = kUCKeyActionDown;
            break;
        case kEventRawKeyUp:
            keyAction = kUCKeyActionUp;
            break;
        case kEventRawKeyRepeat:
            keyAction = kUCKeyActionAutoKey;
            break;
        }
        OSStatus err = UCKeyTranslate(uchrData, keyCode, keyAction,
                                  ((currentModifiersWOAltOrControl >> 8) & 0xff), LMGetKbdType(),
                                  kUCKeyTranslateNoDeadKeysMask, &tmp_unused_state, 4, &actualLength,
                                  string);
        if(err == noErr) {
            *outChar = QChar(string[0]);
            *qtKey = qt_mac_get_key(*outModifiers, *outChar, keyCode);
            if (currentModifiersWOAltOrControl != currentModifiers) {
                // Now get the real char.
                err = UCKeyTranslate(uchrData, keyCode, keyAction,
                                     ((currentModifiers >> 8) & 0xff), LMGetKbdType(),
                                      kUCKeyTranslateNoDeadKeysMask, &tmp_unused_state, 4, &actualLength,
                                      string);
                if(err == noErr)
                    *outChar = QChar(string[0]);
            }
        } else {
            qWarning("Qt::internal::UCKeyTranslate is returnining %ld %s:%d",
                     long(err), __FILE__, __LINE__);
        }
    }
#ifdef Q_OS_MAC32
    else {
        // The road less travelled; use KeyTranslate
        const void *keyboard_layout;
        KeyboardLayoutRef keyLayoutRef = 0;
        KLGetCurrentKeyboardLayout(&keyLayoutRef);
        err = KLGetKeyboardLayoutProperty(keyLayoutRef, kKLKCHRData,
                                  reinterpret_cast<const void **>(&keyboard_layout));

        int translatedChar = KeyTranslate(keyboard_layout, (GetCurrentEventKeyModifiers() &
                                                             (kEventKeyModifierNumLockMask|shiftKey|cmdKey|
                                                              rightShiftKey|alphaLock)) | keyCode,
                                           &tmp_unused_state);
        if(!translatedChar) {
            if(outHandled) {
                qt_mac_eat_unicode_key = false;
                CallNextEventHandler(er, keyEvent);
                *outHandled = qt_mac_eat_unicode_key;
            }
            return false;
        }

        //map it into qt keys
        *qtKey = qt_mac_get_key(*outModifiers, QChar(translatedChar), keyCode);
        if(*outModifiers & (Qt::AltModifier | Qt::ControlModifier)) {
            if(translatedChar & (1 << 7)) //high ascii
                translatedChar = 0;
        } else {          //now get the real ascii value
            UInt32 tmp_mod = 0L;
            static UInt32 tmp_state = 0L;
            if(*outModifiers & Qt::ShiftModifier)
                tmp_mod |= shiftKey;
            if(*outModifiers & Qt::MetaModifier)
                tmp_mod |= controlKey;
            if(*outModifiers & Qt::ControlModifier)
                tmp_mod |= cmdKey;
            if(GetCurrentEventKeyModifiers() & alphaLock) //no Qt mapper
                tmp_mod |= alphaLock;
            if(*outModifiers & Qt::AltModifier)
                tmp_mod |= optionKey;
            if(*outModifiers & Qt::KeypadModifier)
                tmp_mod |= kEventKeyModifierNumLockMask;
            translatedChar = KeyTranslate(keyboard_layout, tmp_mod | keyCode, &tmp_state);
        }
        {
            ByteCount unilen = 0;
            if(GetEventParameter(keyEvent, kEventParamKeyUnicodes, typeUnicodeText, 0, 0, &unilen, 0)
                    == noErr && unilen == 2) {
                GetEventParameter(keyEvent, kEventParamKeyUnicodes, typeUnicodeText, 0, unilen, 0, outChar);
            } else if(translatedChar) {
                static QTextCodec *c = 0;
                if(!c)
                    c = QTextCodec::codecForName("Apple Roman");
		char tmpChar = static_cast< char >( translatedChar ); // **sigh**
                *outChar = c->toUnicode(&tmpChar, 1).at(0);
            } else {
                *qtKey = qt_mac_get_key(*outModifiers, QChar(translatedChar), keyCode);
            }
        }
    }
#endif
    if(*qtKey == Qt::Key_unknown)
        *qtKey = qt_mac_get_key(*outModifiers, *outChar, keyCode);
    return true;
}

static void qt_mac_send_modifiers_changed(quint32 modifiers, QObject *object)
{
    static quint32 cachedModifiers = 0;
    quint32 lastModifiers = cachedModifiers,
          changedModifiers = lastModifiers ^ modifiers;
    cachedModifiers = modifiers;

    //check the bits
    static qt_mac_enum_mapper modifier_key_symbols[] = {
        { shiftKeyBit, QT_MAC_MAP_ENUM(Qt::Key_Shift) },
        { rightShiftKeyBit, QT_MAC_MAP_ENUM(Qt::Key_Shift) }, //???
        { controlKeyBit, QT_MAC_MAP_ENUM(Qt::Key_Meta) },
        { rightControlKeyBit, QT_MAC_MAP_ENUM(Qt::Key_Meta) }, //???
        { cmdKeyBit, QT_MAC_MAP_ENUM(Qt::Key_Control) },
        { optionKeyBit, QT_MAC_MAP_ENUM(Qt::Key_Alt) },
        { rightOptionKeyBit, QT_MAC_MAP_ENUM(Qt::Key_Alt) }, //???
        { alphaLockBit, QT_MAC_MAP_ENUM(Qt::Key_CapsLock) },
        { kEventKeyModifierNumLockBit, QT_MAC_MAP_ENUM(Qt::Key_NumLock) },
        {   0, QT_MAC_MAP_ENUM(0) } };
    for(int i = 0; i <= 32; i++) { //just check each bit
        if(!(changedModifiers & (1 << i)))
            continue;
        QEvent::Type etype = QEvent::KeyPress;
        if(lastModifiers & (1 << i))
            etype = QEvent::KeyRelease;
        int key = 0;
        for(uint x = 0; modifier_key_symbols[x].mac_code; x++) {
            if(modifier_key_symbols[x].mac_code == i) {
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
                qDebug("got modifier changed: %s", modifier_key_symbols[x].desc);
#endif
                key = modifier_key_symbols[x].qt_code;
                break;
            }
        }
        if(!key) {
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
            qDebug("could not get modifier changed: %d", i);
#endif
            continue;
        }
#ifdef DEBUG_KEY_BINDINGS_MODIFIERS
        qDebug("KeyEvent (modif): Sending %s to %s::%s: %d - 0x%08x",
               etype == QEvent::KeyRelease ? "KeyRelease" : "KeyPress",
               object ? object->metaObject()->className() : "none",
               object ? object->objectName().toLatin1().constData() : "",
               key, (int)modifiers);
#endif
        QKeyEvent ke(etype, key, qt_mac_get_modifiers(modifiers ^ (1 << i)), QLatin1String(""));
        QCoreApplication::sendEvent( object, &ke );
        //qt_sendSpontaneousEvent(object, &ke);
    }
}

MacCarbonApplication::MacCarbonApplication( int& argc, char* argv[] )
    : QApplication( argc, argv )
{
}

MacCarbonApplication::~MacCarbonApplication()
{
}

static QList< QShortcut* > shortcuts( QWidget* parent = 0 ) 
{
    QList< QShortcut* > result;
    if( parent == 0 )
    {
        const QWidgetList widgets = QApplication::topLevelWidgets();
        for( QWidgetList::const_iterator it = widgets.begin(); it != widgets.end(); ++it )
            result += shortcuts( *it ); 
    }
    else
    {
        const QList< QShortcut* > cuts = parent->findChildren< QShortcut* >();
        for( QList< QShortcut* >::const_iterator it = cuts.begin(); it != cuts.end(); ++it )
            if( (*it)->context() == Qt::ApplicationShortcut )
                result.push_back( *it );
        
        const QList< QWidget* > children = parent->findChildren< QWidget* >();
        for( QList< QWidget* >::const_iterator it = children.begin(); it != children.end(); ++it )
            result += shortcuts( *it );
    }
    return result;
}

static QList< QShortcut* > activeShortcuts( const QKeySequence& seq, bool autorep, QWidget* parent = 0 )
{
    const QList< QShortcut* > cuts = shortcuts( parent );
    QList< QShortcut* > result;
    for( QList< QShortcut* >::const_iterator it = cuts.begin(); it != cuts.end(); ++it )
        if( (*it)->context() == Qt::ApplicationShortcut && ((*it)->autoRepeat() == autorep || !autorep ) && (*it)->isEnabled() && (*it)->key().matches( seq ) )
            result.push_back( *it );
    return result;
}

bool MacCarbonApplication::macEventFilter( EventHandlerCallRef caller, EventRef event )
{
    switch( GetEventClass( event ) )
    {
    case kEventClassKeyboard:
    {
        if( GetEventKind( event ) == kEventRawKeyModifiersChanged )
        {
            UInt32 modifiers = 0;
            GetEventParameter( event, kEventParamKeyModifiers, typeUInt32, 0, sizeof( modifiers ), 0, &modifiers );
            qt_mac_send_modifiers_changed( modifiers, this );
            break;
        }
       
        Qt::KeyboardModifiers modifiers;
        int qtKey;
        QChar ourChar;
        bool handled;
        if( !translateKeyEventInternal( caller, event, &qtKey, &ourChar, &modifiers, &handled ) )
            return false;
     
        const bool autorep = GetEventKind( event ) == kEventRawKeyRepeat;
        if( GetEventKind( event ) == kEventRawKeyUp )
            break;
        
        const QKeySequence seq( qtKey | modifiers );
        const QList< QShortcut* > active = activeShortcuts( seq, autorep );
        for( QList< QShortcut* >::const_iterator it = active.begin(); it != active.end(); ++it )
        {
            QShortcutEvent event( seq, (*it)->id() );
            QObject* const receiver = *it;
            receiver->event( &event ); 
        }
        return !active.isEmpty();
    }
    case kEventClassAppleEvent:
        emit dockIconClicked();
    }
    return QApplication::macEventFilter( caller, event );
}

#include "MacCarbonApplication.moc"
