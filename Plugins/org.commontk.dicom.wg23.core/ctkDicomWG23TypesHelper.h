

class ctkDicomSoapRectangle (QtSoapStruct) {

    ctkDicomSoapRectangle(Const QtSoapName & name,Const QRect rect):QtSoapStruct(){

    };

    static QRect getQRect (Const QtSoapType& type);
};
