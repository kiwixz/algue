<template>
  <input type="text" v-model="summoner_name" v-on:keyup.enter="go" placeholder="summoner name" autofocus />
</template>

<style>
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  @Component
  export default class extends Vue {
    summoner_name = "";

    async go() {
      // console.debug(
      //   await (
      //     await fetch("/api/is_live", {
      //       method: "POST",
      //       headers: { "content-type": "application/json" },
      //       body: JSON.stringify({ summoner: this.summoner_name }),
      //     })
      //   ).json()
      // );

      window.location.href = `/live/${this.summoner_name}`;
    }
  }
</script>
