<template>
  <div class="app">
    <input type="text" v-model="summoner_name" v-on:keyup.enter="go" placeholder="summoner name" autofocus />
    <Footer></Footer>
  </div>
</template>

<style lang="scss">
  .app {
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

      &:focus {
        outline: unset;
      }

      &::placeholder {
        color: #ddd;
        font-size: 1.4em;
      }
    }
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  import Api from "Common/api";
  import Footer from "Common/footer.vue";

  import "Common/error_toasts";

  @Component({
    components: {
      Footer,
    },
  })
  export default class extends Vue {
    summoner_name = "";

    async go() {
      if (!this.summoner_name)
        return;

      const is_live = await Api.is_live(this.summoner_name);
      if (!is_live.exists) {
        Vue.$toast.error(`summoner ${this.summoner_name} does not exist`);
        return;
      }

      if (is_live.in_game)
        window.location.href = `/live/${encodeURI(this.summoner_name)}`;
      else
        Vue.$toast.error(`summoner ${this.summoner_name} is not in-game`);
    }
  }
</script>
