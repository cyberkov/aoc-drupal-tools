# Howto setup the whole thing.

# Introduction

I assume you already have Bebot and a brand new Drupal 5.x Installation running on your server.
If you haven't go and get them at their respective site.

It is absolutely neccesary that the mysql users are the same or have access to both databases (bebot and drupal, whatever you name them to).

# Setting up the profile fields

There will be some setup routine in the near future. Currently you have to setup the database tables on your own:

execute the following SQL Queries on your Drupal Table:
```
INSERT INTO profile_fields (`title`, `name`, `explanation`, `category`, `page`, `type`, `weight`, `required`, `register`, `visibility`, `autocomplete`, `options`) VALUES
        ('Mainnchar', 'profile_mainchar', 'Name deines Hauptcharakters', 'InGame', '', 'textfield', 0, 1, 1, 2, 0, ''),
        ('Rasse', 'profile_race', 'Abstammung des Charakters', 'InGame', '%value', 'selection', 0, 1, 1, 2, 0, 'Aquilonier\r\nCimmerier\r\nStygier'),
        ('Klasse', 'profile_class', 'Klasse deines Mainchars', 'InGame', '%value', 'selection', 0, 1, 1, 2, 0, 'Assassine\r\nBarbar\r\nBaerenschamane\r\nDaemonologe\r\nDunkler Templer\r\nEroberer\r\nHerold des Xotli\r\nMitrapriester\r\nNekromant\r\nWaechter\r\nVollstrecker Sets\r\nWaldlaeufer'),
        ('Beruf 1', 'profile_profession1', 'Dein erster Beruf, sofern bereits vorhanden.', 'InGame', '', 'selection', 0, 0, 1, 2, 0, 'Waffenschmied\r\nRüstungsschmied\r\nAlchemist\r\nArchitekt\r\nJuwelenschneider'),
        ('Beruf 2', 'profile_profession2', 'Dein zweiter Beruf, sofern bereits vorhanden.', 'InGame', '', 'selection', 0, 0, 1, 2, 0, 'Waffenschmied\r\nRüstungsschmied\r\nAlchemist\r\nArchitekt\r\nJuwelenschneider');
```
(ah yes. The values are currently in german and there is no way for internationalization. I'll try to circumvent that issue in an upcoming release)

# Activating the Bebot reminder module
Place the Bebot/forumregister.php in your Bebot's custom/modules folder and your good to go. 
If you haven't installed Drupal in the database named "drupal", change this to the correct value. A search and replace on "Drupal" should do the job :-)

That's it. Start up Bebot and it will work. If you run in any trouble, please let me know.


# Setting up automatic guild authorization
  * Activate the authorization module at Administer => Modules.
  * Go to Administer => AoC Authorization and set your Bebot Table's name. Normally you're good to go with the gefault value.
  * Do not forget to set the correct role which should be granted on a match.
  * Click save and watch the magic happen :-)
