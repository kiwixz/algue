<template>
  <div>
    this is live, <a href="/">go back to home</a>
    {{ game }}
  </div>
</template>

<style>
  div {
    color: green;
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  @Component
  export default class extends Vue {
    summoner_name;
    game;

    async mounted() {
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
