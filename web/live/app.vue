<template>
  <div>
    <div v-if="game">
      <Team :players="game.players['blue']"></Team>
      <Team :players="game.players['red']"></Team>
    </div>
    <div v-else>loading...</div>
  </div>
</template>

<style></style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  import Team from "./team.vue";

  @Component({
    components: {
      Team,
    },
  })
  export default class extends Vue {
    summoner_name = "";
    game = {};

    async created() {
      const path = window.location.pathname;
      this.summoner_name = path.substring(path.lastIndexOf("/") + 1);
      this.game = await (
        await fetch("/api/live_game", {
          method: "POST",
          headers: { "content-type": "application/json" },
          body: JSON.stringify({ summoner: this.summoner_name }),
        })
      ).json();
    }
  }
</script>
