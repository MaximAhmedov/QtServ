Программа работает только на ос Windows! кроссплатформенность не доработана.
Серверная часть чата для обмена сообщениями между пользователями.
Единовременный доступ реализован через функцию select().
На данном этапе сервер может принимать запрос от пользователя, обрабатывать его и возвращать ответ.
Данные пользователей и сообщения хранятся в базе данных, теперь после завершения работы сервера данные не исчезают.

UPD: добавлен интерфейс, в котором админ может просмотреть все сообщения любого пользователя, так же есть возможность забанить/разбанить пользователя.
БД реазлизована с помощью QSQLITE, функция обработки запросов от пользователей вынесена в отдельный поток.
