<template>
  <div>
    <div class="section name">{{ player.summoner_name }}</div>
    <div class="section champion">
      <img :src="champion.image" />
      <div class="champion-name">{{ champion.name }}</div>
    </div>
    <div class="section">role</div>
    <div class="section">ranked</div>
    <div class="section">history</div>
    <div class="section">tips</div>
  </div>
</template>

<style lang="scss">
  .player {
    border-radius: 4px;

    display: flex;
    flex-flow: column wrap;

    background-color: #222;

    .section {
      margin: 1px 0px;
      padding: 4px;

      background-color: #444;
    }

    .name {
      width: 14em;

      background-color: #222;
      font-size: 1.1em;
      font-weight: 500;
      text-align: center;
    }

    .champion {
      display: flex;

      img {
        display: block;
        width: 60px;

        border-radius: 3px;
      }

      .champion-name {
        margin: 0.2em 0px 0px 0.5em;
      }
    }
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Prop } from "vue-property-decorator";

  import Champions from "Riot/champions";

  @Component
  export default class extends Vue {
    @Prop() readonly player: any;

    get champion() {
      return Champions[this.player.champion];
    }
  }
</script>
