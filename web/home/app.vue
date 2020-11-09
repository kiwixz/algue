<template>
  <input type="text" v-model="summoner_name" v-on:keyup.enter="go" placeholder="summoner name" autofocus />
</template>

<style lang="scss">
  input {
    width: clamp(40vw, 800px, 90vw);
    position: absolute;
    top: 50% - 10%;
    left: 50%;
    transform: translate(-50%, -50%);
    padding: 0.4% 0.8% 0.5%;

    border: 0;
    border-radius: 0.2em;
    font-size: min(2em, 5vw);
    text-align: center;

    &::placeholder {
      color: #ddd;
      font-size: 1.4em;
    }
  }
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
