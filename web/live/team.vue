<template>
  <div>
    <Player v-for="p in players" :key="p.summoner_name" :player="p"></Player>
  </div>
</template>

<style></style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Prop } from "vue-property-decorator";

  import Player from "./player.vue";

  @Component({
    components: {
      Player,
    },
  })
  export default class extends Vue {
    @Prop() readonly players;
  }
</script>
