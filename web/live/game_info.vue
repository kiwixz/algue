<template>
  <div class="game_info">
    <div>
      {{ queue_type.name }}<span class="map">({{ queue_type.map }})</span>
    </div>
    <div>bans</div>
  </div>
</template>

<style lang="scss">
  .game_info {
    display: flex;
    flex-wrap: wrap;
    justify-content: space-between;

    .map {
      margin-left: 1em;

      color: #555;
    }
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Prop } from "vue-property-decorator";

  import Queues from "Riot/queues";

  @Component
  export default class extends Vue {
    @Prop() readonly game: any;

    get queue_type() {
      return Queues[this.game.queue_type];
    }
  }
</script>
