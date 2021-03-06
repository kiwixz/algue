# web

## home
- smart selection to go /live or /history depending if is currently in-game

## /live
- ranks
- previous encounters (number of times played with and against)
    - show a little icons next to name
    - colored icon for same team premades
        - yellow then green for red team
        - cyan then green for blue team
    - magenta icon when there is inter-team relations
    - hovering it shows detailed informations
- wait/refresh when summoner not yet in-game
- search bar to change player
- swap two players to consider different roles
- mastery points and level on current champion
- average kda on current champion
- runes
- match history
- summoner spells
- usual champions
- usual roles
- current role estimation
- add remarks

### lobby preview (with list of players)
- access with /live/mainplayer,player2,player3
- wait/refresh to see when original player is in-game
- add a landing page for lobby preview where you can input names

## /history
- redirect to home on error
- list matches
- detailed view of a match
- champion mastery levels and points
- "user stats" stuff also calculated for live game analysis
- most played champions/roles
- ranks
- winrates
- player remarks
- current champion pick frequency
- warding
- objective focus (splitpush/dragons)

## /stats
- champions
    - global and for each below: winrate, kda, minions, gold
    - counters
    - summoner spells
    - runes
    - spells with order
    - items with order
    - roles distribution ?
- items winrate
    - global
    - per champion

## riot data
- custom image conversion after download
    - jpeg output
    - avif
- do not download the full archive


# daemon
- async http client
- riot client async response
- riot client api rate limit (global and per endpoint, via headers)
- cache api results in sqlite db
    - create a db in-memory
    - add a table for each api endpoint used
    - index by input parameters of the api (ex: summoner name) and timestamp
    - store json response as string
    - occasional cleanup
- http client reuse socket
- http client multiple socket
- add timeout to http requests

## global stats sniffing
- analyze plat+ matchs to db
- tool to locally run custom sql on db
- relevant information from current match/player history
- consider making custom item sets (supersede leitse)


# local lcu helper
- method to build lcu api request
- add websocket support
- listen to events
- open/refresh website on lobby/party changes
- autolobby: accept match, preselect champ, ban, pick


# misc
- fix json overflow handling when parsing (current way of casting is probably UB)
- use pmr containers for json parsing
- abort on ubsan error in unittest
