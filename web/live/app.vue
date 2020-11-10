<template>
  <div v-if="game" class="app">
    <Header :summoner_name="summoner_name"></Header>
    <div class="container">
      <div>
        <GameInfo :game="game"></GameInfo>
        <Team :players="game.players[riot_teams.red]"></Team>
        <Team :players="game.players[riot_teams.blue]"></Team>
      </div>
    </div>
  </div>
  <div v-else>loading...</div>
</template>

<style lang="scss">
  .app {
    .container {
      margin-top: 20px;

      display: flex;
      justify-content: center;
    }
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  import RiotTeams from "Riot/teams";

  import GameInfo from "./game_info.vue";
  import Header from "./header.vue";
  import Team from "./team.vue";

  @Component({
    components: {
      Header,
      GameInfo,
      Team,
    },
  })
  export default class extends Vue {
    summoner_name = "";
    game: any = null;

    get riot_teams() {
      return RiotTeams;
    }

    async created() {
      const path = window.location.pathname;
      this.summoner_name = decodeURI(path.substring(path.lastIndexOf("/") + 1));
      this.game = await (
        await fetch("/api/live_game", {
          method: "POST",
          headers: { "content-type": "application/json" },
          body: JSON.stringify({ summoner: this.summoner_name }),
        })
      ).json();
      document.title = `algue / live / ${this.summoner_name}`;
    }
  }
</script>
