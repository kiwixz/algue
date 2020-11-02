<template>
  <div class="player">
    <div class="player-name">
      {{ player.summoner_name }}
    </div>
  </div>
</template>

<style>
  .player {
    padding: 10px;
    border-radius: 4px;

    background-color: #222;
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Prop } from "vue-property-decorator";

  @Component
  export default class extends Vue {
    @Prop() readonly player: object | undefined;
  }
</script>
