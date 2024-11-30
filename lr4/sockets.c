/* процедуры reads() и writes() для чтения и записи в сокет
   в режиме виртуального соединения  */

/* запись в сокет буфера, состоящего из пос байт */
ssize_t writes(int sock, void *pbuf, size_t bufferSize) {
    ssize_t bytesToWrite, result;
    bytesToWrite = (ssize_t) bufferSize;

    while (bytesToWrite > 0) {
        result = write(sock, pbuf, bytesToWrite);

        if (result < 0) {
            return result;
        }

        bytesToWrite -= result;
        pbuf += result;
    }

    return ((ssize_t) bufferSize - bytesToWrite);
}

/* считывание в буфер noc байт из сокета     */
ssize_t reads(int sock, void *pbuf, size_t noc) {
    ssize_t bytesToRead, result;

    bytesToRead = (ssize_t) noc;
    while (bytesToRead > 0) {
        result = read(sock, pbuf, bytesToRead);

        if (result < 0) {
            return (result);
        } else if (result == 0) {
            break;
        }

        bytesToRead -= result;
        pbuf += result;
    }

    return ((ssize_t) noc - bytesToRead);
}                                                          
