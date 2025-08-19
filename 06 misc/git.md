# Git основные команды

Создание пользователя
```
git config --global user.name "ezexff"
git config --global user.email "EMAIL"
```

Авторизация через гитхаб
```
git config --global github.user "ezexff"
git config --global github.token "TOKEN"
```

Добавить репозиторий на гитхаб
```
git init
git add .
git commit -m "1st commit"
git branch -M master
git remote add origin https://github.com/ezexff/engine.git
git push -u origin master
```

Обновить на гитхабе репозиторий
```
git push
```

Обновить репозиторий по содержимому на гитхабе
```
git pull
```

Посмотреть настройки
```
git config --list
```

Посмотреть помощь
```
git help commit
```

Инициализация в текущей папке git через git bash
```
git init
```

Посмотреть статус репозитория
```
git status
```

Добавить все файлы
```
git add .
```

Сохранить изменения, где -m сообщение
```
git commit -m "first commit"
```

Информация о изменениях
```
git log
```

Клонировать репозиторий
```
git clone "url без кавычек"
```

Запустить git gui из git bash
```
git gui&
```

Запустить git k из git bash
```
gitk&
```

Добавить папку build в git ignore
```
echo build > .gitignore
```

Отложить текущие изменения в хранилище
```
git stash
```

Удаление отложенных изменений
```
git stash drop
```

Число строк кода
```
git ls-files | xargs wc -l
```
