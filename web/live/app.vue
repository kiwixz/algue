<template>
  <div class="app">
    <div v-if="game">
      <Header :summoner_name="summoner_name"></Header>
      <div class="container">
        <div>
          <GameInfo :game="game"></GameInfo>
          <Team class="red_team" :players="game.players[riot_teams.red]"></Team>
          <Team class="blue_team" :players="game.players[riot_teams.blue]"></Team>
        </div>
      </div>
    </div>
    <div v-else>loading...</div>
    <Footer></Footer>
  </div>
</template>

<style lang="scss">
  .app {
    .container {
      margin-top: 20px;

      display: flex;
      justify-content: center;

      .red_team .player {
        background-color: #422;
      }

      .blue_team .player {
        background-color: #224;
      }
    }
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  import Api from "Common/api";
  import Footer from "Common/footer.vue";
  import RiotTeams from "Riot/teams";

  import GameInfo from "./game_info.vue";
  import Header from "./header.vue";
  import Team from "./team.vue";

  import "Common/error_toasts";

  @Component({
    components: {
      Footer,
      GameInfo,
      Header,
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
      document.title = `algue / live / ${this.summoner_name}`;
      this.game = await Api.live_game(this.summoner_name);
    }
  }
</script>
