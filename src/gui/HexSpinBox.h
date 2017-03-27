/**********************************************************
 * Source: http://stackoverflow.com/questions/26581444/qspinbox-with-unsigned-int-for-hex-input
 * Author: ZX2C4
 * ********************************************************/
#ifndef HEXSPINBOX_H
#define HEXSPINBOX_H

#include <QSpinBox>

class HexSpinBox : public QSpinBox
{
    public:
        HexSpinBox(bool only16Bits = false, QWidget *parent = 0) : QSpinBox(parent), m_only16Bits(only16Bits){
        setPrefix("0x");
        setDisplayIntegerBase(16);
        if (only16Bits){
            setRange(0, 0xFFFFFF);
            setSuffix("XX");
        }
        else
            setRange(INT_MIN, INT_MAX);
    }
        unsigned int hexValue() const
        {
            return u(value());
        }
        void setHexValue(unsigned int value)
        {
            setValue(i(value));
        }
    protected:
        QString textFromValue(int value) const
        {
            if(m_only16Bits)
                return QString::number(u(value), 16).toUpper().rightJustified(6,'0');
            else
                return QString::number(u(value), 16).toUpper().rightJustified(8,'0');

        }
        int valueFromText(const QString &text) const
        {
            return i(text.toUInt(0, 16));
        }
        QValidator::State validate(QString &input, int &pos) const
        {
            QString copy(input);
            if (copy.startsWith("0x"))
                copy.remove(0, 2);
            pos -= copy.size() - copy.trimmed().size();
            copy = copy.trimmed();
            if (copy.isEmpty())
                return QValidator::Intermediate;
            input = QString("0x") + copy.toUpper();
            bool okay;
            unsigned int val = copy.toUInt(&okay, 16);
            if (!okay || (m_only16Bits && val > 0xFFFFFF))
                return QValidator::Invalid;
            return QValidator::Acceptable;
        }

    private:
        bool m_only16Bits;
        inline unsigned int u(int i) const
        {
            return *reinterpret_cast<unsigned int *>(&i);
        }
        inline int i(unsigned int u) const
        {
            return *reinterpret_cast<int *>(&u);
        }

};

#endif
