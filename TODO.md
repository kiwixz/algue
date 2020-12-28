# web

## home
- handle summoners that dont exist
- smart selection to go /live or /history depending if is currently in-game

## /live
- wait/refresh when summoner not yet in-game
- search bar to change player
- swap two players to consider different roles
- previous encounters (number of times played with and against)
    - show a little icons next to name
    - colored icon for same team premades
        - yellow then green for red team
        - green then yellow for blue team
    - magenta icon when there is inter-team relations
    - hovering it shows detailed informations
- summoner spells
- runes
- ranks
- match history
- average kda on current champion
- mastery points and level on current champion
- usual champions
- usual roles
- current role estimation
- add remarks
- when not in-game, add input for lobby chat messages which opens lobby preview

### lobby preview (with list of players)
- access with /live/mainplayer,player2,player3
- wait/refresh to see when original player is in-game

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


# daemon
- riot client add method with returns status_code instead of throwing
- riot client async response
- riot client api rate limit (global and per endpoint, via headers)
- cache api results in sqlite db
    - create a db in-memory
    - add a table for each api endpoint used
    - index by input parameters of the api (ex: summoner name) and timestamp
    - store json response as string
    - occasional cleanup

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


# misc
- bytes vector child class, with char_begin, char_end, as_string_view
- autolobby: accept match, preselect champ, ban, pick
- async http client
- http client reuse socket
- http client multiple socket
