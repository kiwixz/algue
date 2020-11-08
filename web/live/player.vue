<template>
  <div class="player">
    <div class="player-section player-name">{{ player.summoner_name }}</div>
    <div class="player-section">champion</div>
    <div class="player-section">role</div>
    <div class="player-section">ranked</div>
    <div class="player-section">history</div>
    <div class="player-section">tips</div>
  </div>
</template>

<style>
  .player {
    border-radius: 4px;

    display: flex;
    flex-flow: column wrap;

    background-color: #222;
  }

  .player-section {
    margin: 1px 0px;
    padding: 4px;

    background-color: #444;
  }

  .player-name {
    width: 14em;

    text-align: center;
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
